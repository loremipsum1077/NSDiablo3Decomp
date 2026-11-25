// X/Common/TagMaps/Tags.cpp
// Blank lines keep each function on its original line (TagGetDefinitionDataType asserts with
// __LINE__).

#include "Tags.h"
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "MapSerializationUtils.h"
#include "../BBE.h"
#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../Server/SGameManager.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Sigma.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Definitions/GameBalanceDefinition.h"


#include "../Game.h"

















// An empty tag map is not allocated: its pointer is this pattern with the base's number in the
// low bits (TagMapCreateEmpty). Invented names; the DWARF records no helper for the test.
#define TAGMAP_EMPTY_MASK 0xffee000000000000ull
#define TAGMAP_IS_EMPTY(p) (((uintptr)(p) & TAGMAP_EMPTY_MASK) == TAGMAP_EMPTY_MASK)

inline TagBases TagMapGetTagBase(const CTagMap * pTagMap);
inline int32 TagMapGetDefaultInt(const CTagMap * pTagMap, const uint32 dwTag);
inline float TagMapGetDefaultFloat(const CTagMap * pTagMap, const uint32 dwTag);
inline StringFormula * TagMapGetDefaultStringFormula(const CTagMap * pTagMap, const uint32 dwTag);
inline StringFormula * TagMapGetDefaultStringFormula(const AxeTagInfo * ptInfo);
inline void TagMapSetSNO(CTagMap * pTagMap, uint32 dwTag, SNO snoValue);
static void sTagMapSetValue(CTagMap * pTagMap, uint32 dwTag, TagVariant & variantTagValue);
static void sTagMapSerializeHelper(const CTagMap * pTagMap, void *& pBlockPtr, int32 & nBlockSize, uint32 dwFlags);





static Blizzard::Lock::RMutex stTagMapPoolCritical;




// Original line 70 uses the no-argument initializer; lines 74 and 93 configure
// server/client pool counts and growth together.
static void sTagMapInitializePool(TagMapPools * ptTagMapPools, XMemoryAllocatorInterface * ptMemoryInterface,
                                  BOOL fServerTags)
{



    ptTagMapPools->tTagMemoryPool.Initialize();
    ptTagMapPools->tTagMemoryPool.SetMemoryInterface(ptMemoryInterface);

    if (fServerTags)
    {
        ptTagMapPools->tTagMemoryPool.SetBlockCount_Tracked(1024);
        ptTagMapPools->tTagMemoryPool.SetGrow(XALLOC_GROW_LINEAR, 1024);
    }
    else
    {
        ptTagMapPools->tTagMemoryPool.SetBlockCount_Tracked(49152);
        ptTagMapPools->tTagMemoryPool.SetGrow(XALLOC_GROW_LINEAR, 4096);
    }

    ptTagMapPools->tTagMemoryPool.SetShrinkable(TRUE);




    ptTagMapPools->poolTagMaps.Initialize();
    ptTagMapPools->poolTagMaps.SetMemoryInterface(ptMemoryInterface);

    if (fServerTags)
    {
        ptTagMapPools->poolTagMaps.SetBlockCount_Tracked(128);
        ptTagMapPools->poolTagMaps.SetGrow(XALLOC_GROW_LINEAR, 128);
    }
    else
    {
        ptTagMapPools->poolTagMaps.SetBlockCount_Tracked(8192);
        ptTagMapPools->poolTagMaps.SetGrow(XALLOC_GROW_LINEAR, 512);
    }

    ptTagMapPools->poolTagMaps.SetShrinkable(TRUE);

    ptTagMapPools->tGlobalTagMapRef.Initialize(16, FALSE, NULL);
    ptTagMapPools->tGlobalTagMapRef.SetHashTableSize_Tracked(1024);
    ptTagMapPools->tGlobalTagMapRef.SetBlockCount_Tracked(4096);
    ptTagMapPools->tGlobalTagMapRef.SetGrow(XALLOC_GROW_LINEAR, 512);
    ptTagMapPools->tGlobalTagMapRef.SetShrinkable(TRUE);
}







static void sTagMapDisposePool(
    TagMapPools * ptTagMapPools)
{
    if (ptTagMapPools->poolTagMaps.GetUsedBlockCount() != 0)
    {


        ptTagMapPools->poolTagMaps.PrintUsage();


        POS pos = ptTagMapPools->poolTagMaps.GetFirstBlockPOS();
        // DWARF: pTagMap (132) is the loop body's only local; no use of it survives
        while (pos)
        {
            CTagMap * pTagMap = ptTagMapPools->poolTagMaps.GetNextBlock(pos);
        }
    }










    ptTagMapPools->poolTagMaps.Dispose();
    ptTagMapPools->tTagMemoryPool.Dispose();
}




void TagMapInitializeForApp()
{
    XMemoryAllocatorInterface * ptMemoryInterface = NULL;
    AppGlobals & tAppGlobals = sgtAppGlobals;
    TagMapAppGlobals *& ptTagMapAppGlobals = tAppGlobals.ptTagMapAppGlobals;
    ptTagMapAppGlobals = (TagMapAppGlobals *)SigmaMemoryNew(sizeof(TagMapAppGlobals), 4, ptMemoryInterface, TRUE);

    for (int32 i = 0; i < NUM_TAG_BASES; i++)
    {
        TagInfoMap & tTagInfoMap = ptTagMapAppGlobals->tTagInfoMaps[i];




        tTagInfoMap.Initialize(16, FALSE, ptMemoryInterface);
        {
            int32 nCount;
            TagBases eTagBase = (TagBases)((i + 1) << 16);
            const AxeTagInfo * ptTagInfo = GetTagTypeInfo(eTagBase, nCount);
            if (nCount > 0)
            {


                tTagInfoMap.SetHashTableSize_Tracked(nCount * 2);
                for (int32 nIndex = 0; nIndex < nCount; nIndex++)
                {
                    const AxeTagInfo & tTagInfo = ptTagInfo[nIndex];

                    tTagInfoMap.SetAt_Tracked(tTagInfo.dwTagId, &tTagInfo);
                }
            }
        }
    }

    sTagMapInitializePool(&ptTagMapAppGlobals->tAppPools, ptMemoryInterface, FALSE);
}






// NON_MATCHING: in the inlined FixedBlockAllocator::GetNextBlock the original keeps the free-list
// null test of _IsBlockUsed inside the scan loop; ours hoists it (see XFixedBlockAllocator.h).
void TagMapDisposeForApp()

{





    AppGlobals & tAppGlobals = sgtAppGlobals;
    TagMapAppGlobals *& ptTagMapAppGlobals = tAppGlobals.ptTagMapAppGlobals;

    for (int32 i = 0; i < NUM_TAG_BASES; i++)
    {
        TagInfoMap & tTagInfoMap = ptTagMapAppGlobals->tTagInfoMaps[i];
        tTagInfoMap.Dispose();
    }

    sTagMapDisposePool(&ptTagMapAppGlobals->tAppPools);
    SigmaMemoryFree(ptTagMapAppGlobals, NULL);
    ptTagMapAppGlobals = NULL;
}




void TagMapInitializeForGame()
{
    SGame & tSGame = SGameManager::GameGet();

    tSGame.ptTagMapPools = (TagMapPools *)SigmaMemoryNew(sizeof(TagMapPools), 4, tSGame.ptMemoryInterface, TRUE);
    sTagMapInitializePool(tSGame.ptTagMapPools, tSGame.ptMemoryInterface, TRUE);
}






// NON_MATCHING: same inlined FixedBlockAllocator::GetNextBlock loop shape as TagMapDisposeForApp.
void TagMapDisposeForGame()

{

    SGame & tSGame = SGameManager::GameGet();

    sTagMapDisposePool(tSGame.ptTagMapPools);

    SigmaMemoryFree(tSGame.ptTagMapPools, tSGame.ptMemoryInterface);
    tSGame.ptTagMapPools = NULL;
}




static inline TagMapPools * sTagMapGetMemPool()
{
    AppGlobals & tAppGlobals = sgtAppGlobals;
    TagMapAppGlobals * ptTagMapAppGlobals = tAppGlobals.ptTagMapAppGlobals;
    return &ptTagMapAppGlobals->tAppPools;
}

























CTagMap * TagMapCreateEmpty(const TagBases eTagBase)
{
    return (CTagMap *)((uintptr)(eTagBase >> 16) | TAGMAP_EMPTY_MASK);
}










CTagMap * TagMapCreate(const TagBases eTagBase)
{
    TagMapPools * pMemPool = sTagMapGetMemPool();
    Blizzard::Lock::ScopedMutex scopedMutex(stTagMapPoolCritical);
    CTagMap * pTagMap = pMemPool->poolTagMaps.Alloc_Tracked();
    pTagMap->m_dwFlags = 1;
    pTagMap->SetTagBase(eTagBase);
    pTagMap->m_dwRefCount = 1;
    pTagMap->m_TagMap.SetPool(&pMemPool->tTagMemoryPool);
    pTagMap->m_dwCRC = 0;
    return pTagMap;
}













inline BOOL CTagMapRef::operator==(const CTagMapRef & tagMapRef) const
{
    return m_ptTagMap->m_dwCRC == tagMapRef.m_ptTagMap->m_dwCRC && TagMapCompare(m_ptTagMap, tagMapRef.m_ptTagMap);
}





template <>
inline uint32 XHashKey<const CTagMapRef &>(const CTagMapRef & tTagMapRef)
{
    return tTagMapRef.GetTagMap()->m_dwCRC;
}




void TagMapRegisterGlobal(CTagMap * pTagMap)
{
    Blizzard::Lock::ScopedMutex scopedMutex(stTagMapPoolCritical);
    AppGlobals & tAppGlobals = sgtAppGlobals;
    TagMapAppGlobals * ptTagMapAppGlobals = tAppGlobals.ptTagMapAppGlobals;
    CTagMapRef tDummy(pTagMap);
    ptTagMapAppGlobals->tAppPools.tGlobalTagMapRef.SetAt_Tracked(tDummy, tDummy);
}







CTagMap * TagMapFindIdenticalGlobal(CTagMap * pTagMap)
{
    Blizzard::Lock::ScopedMutex scopedMutex(stTagMapPoolCritical);
    AppGlobals & tAppGlobals = sgtAppGlobals;
    TagMapAppGlobals * ptTagMapAppGlobals = tAppGlobals.ptTagMapAppGlobals;
    CTagMapRef tIdentical;
    if (ptTagMapAppGlobals->tAppPools.tGlobalTagMapRef.Lookup(CTagMapRef(pTagMap), tIdentical))
        return tIdentical.GetTagMap();
    return NULL;
}










void TagMapDispose(CTagMap *& pTagMap)
{
    if (pTagMap == NULL)
        return;
    if (TAGMAP_IS_EMPTY(pTagMap))
    {
        pTagMap = NULL;
        return;
    }

    Blizzard::Lock::ScopedMutex scopedMutex(stTagMapPoolCritical);
    pTagMap->m_dwRefCount--;
    if (pTagMap->m_dwRefCount == 0)
    {
        AppGlobals & tAppGlobals = sgtAppGlobals;
        TagMapAppGlobals * ptTagMapAppGlobals = tAppGlobals.ptTagMapAppGlobals;
        CTagMapRef tDummy;
        if (ptTagMapAppGlobals->tAppPools.tGlobalTagMapRef.Lookup(pTagMap, tDummy))
            ptTagMapAppGlobals->tAppPools.tGlobalTagMapRef.RemoveAt(CTagMapRef(pTagMap));
        pTagMap->m_dwFlags &= ~(1 << TAGMAP_FLAG_INITIALIZED);
        TagMapRemoveAll(pTagMap);
        TagMapPools * pMemPool = sTagMapGetMemPool();
        pMemPool->poolTagMaps.Free(pTagMap);
    }
    pTagMap = NULL;
}

























void TagMapDispose(XPtr64<CTagMap> & pTagMap)
{
    if (pTagMap == NULL)
        return;
    if (TAGMAP_IS_EMPTY((CTagMap *)pTagMap))
    {
        pTagMap = NULL;
        return;
    }

    Blizzard::Lock::ScopedMutex scopedMutex(stTagMapPoolCritical);
    pTagMap->m_dwRefCount--;
    if (pTagMap->m_dwRefCount == 0)
    {
        AppGlobals & tAppGlobals = sgtAppGlobals;
        TagMapAppGlobals * ptTagMapAppGlobals = tAppGlobals.ptTagMapAppGlobals;
        CTagMapRef tDummy;
        if (ptTagMapAppGlobals->tAppPools.tGlobalTagMapRef.Lookup(CTagMapRef(pTagMap), tDummy))
            ptTagMapAppGlobals->tAppPools.tGlobalTagMapRef.RemoveAt(CTagMapRef(pTagMap));
        pTagMap->m_dwFlags &= ~(1 << TAGMAP_FLAG_INITIALIZED);
        TagMapRemoveAll(pTagMap);
        TagMapPools * pMemPool = sTagMapGetMemPool();
        pMemPool->poolTagMaps.Free(pTagMap);
        pTagMap = NULL;
    }
}























POS TagMapGetStartPosition(const CTagMap * pTagMap)
{
    if (TAGMAP_IS_EMPTY(pTagMap))
        return NULL;
    return pTagMap->m_TagMap.GetStartAssoc();
}










uint32 TagMapGetNextTag(const CTagMap * pTagMap, POS & pos)
{
    uint32 dwKey;
    TagVariant vValue;
    pTagMap->m_TagMap.GetNextAssoc(pos, dwKey, vValue);
    return dwKey;
}








int32 TagMapGetSerialLength(const CTagMap * pTagMap, uint32 dwFlags)
{
    int32 nBlockSize = 0;
    void * pBlock = NULL;
    sTagMapSerializeHelper(pTagMap, pBlock, nBlockSize, dwFlags);
    return nBlockSize;
}







void TagMapSerialize(const CTagMap * pTagMap, void * pBlock, uint32 dwFlags)
{
    int32 nBlockSize = 0;
    sTagMapSerializeHelper(pTagMap, pBlock, nBlockSize, dwFlags);
}








void TagMapUnserialize(CTagMap * pTagMap, void * pBlock)
{
    if (pBlock == NULL)
        return;

    int32 nMapCount;
    BOOL bReadOnly = (pTagMap->m_dwFlags >> TAGMAP_FLAG_READONLY) & 1;
    MapSerializationUtils::UnserializeInt(pBlock, nMapCount);
    while (nMapCount--)
    {
        uint32 dwTag;
        TagVariantType eTagVariantType;
        TagVariant variantTagValue;
        MapSerializationUtils::UnserializeInt(pBlock, eTagVariantType);
        MapSerializationUtils::UnserializeInt(pBlock, (int32 &)dwTag);
        MapSerializationUtils::UnserializeVariant(eTagVariantType, pBlock, variantTagValue, bReadOnly);









        sTagMapSetValue(pTagMap, dwTag, variantTagValue);
    }
}




const AxeTagInfo * TagMapGetTagInfo(const CTagMap * pTagMap, const uint32 dwTagId)
{
    TagBases eTagBase = TagMapGetTagBase(pTagMap);














    return TagMapGetTagInfo(eTagBase, dwTagId);
}




const AxeTagInfo * TagMapGetTagInfo(const TagBases eTagBase, const uint32 dwTagId)
{
    const AppGlobals & tAppGlobals = sgtAppGlobals;
    const TagMapAppGlobals * ptTagMapAppGlobals = tAppGlobals.ptTagMapAppGlobals;
    const AxeTagInfo * ptTagInfo = NULL;
    int32 nIndex = eTagBase >> 16;







    ptTagMapAppGlobals->tTagInfoMaps[nIndex - 1].Lookup(dwTagId, ptTagInfo);
    return ptTagInfo;
}





BOOL TagMapTagExists(const CTagMap * pTagMap, uint32 dwTag)
{
    if (TAGMAP_IS_EMPTY(pTagMap))
        return FALSE;

    TagVariant variantTagValue;
    return pTagMap->m_TagMap.Lookup(dwTag, variantTagValue);
}



























void TagMapRemoveTag(CTagMap * pTagMap, uint32 dwTag)
{
    if (TAGMAP_IS_EMPTY(pTagMap))
        return;

    TagVariant variantTagValue;
    if (pTagMap->m_TagMap.Lookup(dwTag, variantTagValue))
    {
        TagVariantType eVariantType = TagGetVariantType(pTagMap, dwTag);
        MapSerializationUtils::FreeVariant(eVariantType, variantTagValue);
        Blizzard::Lock::ScopedMutex scopedMutex(stTagMapPoolCritical);
        pTagMap->m_TagMap.RemoveAt(dwTag);
    }
}

















void TagMapRemoveAll(CTagMap * pTagMap)
{
    if (TAGMAP_IS_EMPTY(pTagMap))
        return;

    if (!(pTagMap->m_dwFlags & (1 << TAGMAP_FLAG_READONLY)))
    {
        POS pos = pTagMap->m_TagMap.GetStartAssoc();
        while (pos)
        {
            uint32 dwTag;
            TagVariantType eVariantType;
            TagVariant variantTagValue;
            pTagMap->m_TagMap.GetNextAssoc(pos, dwTag, variantTagValue);
            eVariantType = TagGetVariantType(pTagMap, dwTag);
            MapSerializationUtils::FreeVariant(eVariantType, variantTagValue);
        }
    }

    Blizzard::Lock::ScopedMutex scopedMutex(stTagMapPoolCritical);
    pTagMap->m_TagMap.RemoveAll();
}













int32 TagMapGetTagCount(const CTagMap * pTagMap)
{
    if (TAGMAP_IS_EMPTY(pTagMap))
        return 0;
    return pTagMap->m_TagMap.Count();
}









void TagMapIterateSnoReferences(const CTagMap * pTagMap, SNOIterateFunc pfuncIterate, void * pData)
{
    if (TAGMAP_IS_EMPTY(pTagMap))
        return;

    POS pos = pTagMap->m_TagMap.GetStartAssoc();
    while (pos)
    {
        uint32 dwTag;
        SNOName snoname;
        TagVariant variantTagValue;
        pTagMap->m_TagMap.GetNextAssoc(pos, dwTag, variantTagValue);
        if (AxeTagGetSNOName(pTagMap, dwTag, snoname))
        {
            uint32 dwFlags = 1;
            const AxeTagInfo * ptInfo = TagMapGetTagInfo(pTagMap, dwTag);
            dwFlags |= ptInfo->nFlags & (1 << AXETAGFLAG_SOFT_LINK);
            pfuncIterate(snoname, dwFlags, pData);
        }
    }
}

















void TagMapChangeSNOReference(CTagMap * pTagMap, const SNOName & snonameOld, const SNO snoNew)
{
    if (TAGMAP_IS_EMPTY(pTagMap))
        return;

    POS pos = pTagMap->m_TagMap.GetStartAssoc();
    while (pos)
    {
        uint32 dwTag;
        SNOName snoname;
        TagVariant variantTagValue;
        pTagMap->m_TagMap.GetNextAssoc(pos, dwTag, variantTagValue);
        if (AxeTagGetSNOName(pTagMap, dwTag, snoname) && snonameOld == snoname)
            TagMapSetSNO(pTagMap, dwTag, snoNew);
    }
}

















inline TagBases TagMapGetTagBase(const CTagMap * pTagMap)
{
    if (TAGMAP_IS_EMPTY(pTagMap))
        return (TagBases)((uint32)(uintptr)pTagMap << 16);
    return pTagMap->GetTagBase();
}












TagVariantType TagGetVariantType(const AxeTagType eType)
{
    return TagGetDefinitionDataType(eType)->GetTagVariantType();
}





TagVariantType TagGetVariantType(const CTagMap * pTagMap, const uint32 dwTag)
{
    const AxeTagInfo * ptInfo = TagMapGetTagInfo(pTagMap, dwTag);
    return TagGetVariantType(ptInfo->eType);
}









const XTypeDescriptor * TagGetDefinitionDataType(const AxeTagType eType)
{
    const XTypeDescriptor * ptType = NULL;
    switch (eType)
    {
    case AXETAGTYPE_FLOAT:
    case AXETAGTYPE_HIGH_PRECISION_FLOAT:
    case AXETAGTYPE_HIGH_PRECISION_PERCENT:
        ptType = DT_FLOAT;
        break;
    case AXETAGTYPE_ANIM:
    case AXETAGTYPE_SHADER:
    case AXETAGTYPE_ACTOR:
    case AXETAGTYPE_STRINGLIST:
    case AXETAGTYPE_SOUND:
    case AXETAGTYPE_SOUNDBANK:
    case AXETAGTYPE_AMBIENT_SOUND:
    case AXETAGTYPE_POWER:
    case AXETAGTYPE_PARTICLE:
    case AXETAGTYPE_TEXTURE:
    case AXETAGTYPE_EFFECTGROUP:
    case AXETAGTYPE_WORLD:
    case AXETAGTYPE_CLOTH:
    case AXETAGTYPE_ROPE:
    case AXETAGTYPE_MONSTER:
    case AXETAGTYPE_SNO_LEVEL_AREA:
    case AXETAGTYPE_WEATHER:
    case AXETAGTYPE_SHAKE:
    case AXETAGTYPE_ANIM2D_SNO:
    case AXETAGTYPE_ENCOUNTER:
    case AXETAGTYPE_SCRIPT:
    case AXETAGTYPE_CONVERSATION:
    case AXETAGTYPE_CONVERSATIONLIST:
    case AXETAGTYPE_TREASURECLASS:
    case AXETAGTYPE_OBSERVER:
    case AXETAGTYPE_LORE:
    case AXETAGTYPE_AISTATE:
    case AXETAGTYPE_CONDITION:
    case AXETAGTYPE_CONDUCTOR:
    case AXETAGTYPE_TIMEDEVENT:
    case AXETAGTYPE_QUESTRANGE:
    case AXETAGTYPE_SURFACE:
    case AXETAGTYPE_REVERB:
    case AXETAGTYPE_MUSIC:
    case AXETAGTYPE_SNO_BOSS_ENCOUNTER:
    case AXETAGTYPE_VIBRATION:
    case AXETAGTYPE_SNO_QUEST:
        ptType = DT_SNO;
        break;
    case AXETAGTYPE_INT:
    case AXETAGTYPE_BRAIN:
    case AXETAGTYPE_GIZMO:
    case AXETAGTYPE_HITSOUND:
    case AXETAGTYPE_ANIM_TAG:
    case AXETAGTYPE_BOOL:
    case AXETAGTYPE_GENDER:
    case AXETAGTYPE_WEAPON_CLASS:
    case AXETAGTYPE_COMPONENTSTYLE:
    case AXETAGTYPE_COMPONENTLOOK:
    case AXETAGTYPE_ACTOR_TOPOLOGY:
    case AXETAGTYPE_ITEM_IMAGE:
    case AXETAGTYPE_MAP_IMAGE:
    case AXETAGTYPE_UI_IMAGE:
    case AXETAGTYPE_USEITEM:
    case AXETAGTYPE_GIZMO_LABEL:
    case AXETAGTYPE_GAME_ERROR_LABEL:
    case AXETAGTYPE_MINIMAP_NAME_LABEL:
    case AXETAGTYPE_SET_DUNGEON_DESCRIPTION_LABEL:
    case AXETAGTYPE_SET_DUNGEON_TITLE_LABEL:
    case AXETAGTYPE_HARDPOINT:
    case AXETAGTYPE_WORLD_RULE_TYPE:
    case AXETAGTYPE_WORLD_PLACEMENT_TYPE:
    case AXETAGTYPE_WORLD_TILE_TYPE:
    case AXETAGTYPE_WORLD_TILE_SET:
    case AXETAGTYPE_WORLD_TILE_FAMILY:
    case AXETAGTYPE_VERTALPHA_FUNCTION:
    case AXETAGTYPE_INVENTORY_TYPE:
    case AXETAGTYPE_PASSABILTY_TYPE:
    case AXETAGTYPE_DOOR_NUMBER:
    case AXETAGTYPE_BEHAVIOR:
    case AXETAGTYPE_TEXCOORD_FUNC:
    case AXETAGTYPE_MATERIAL_FUNC:
    case AXETAGTYPE_PS_FUNC:
    case AXETAGTYPE_PMA_FUNC:
    case AXETAGTYPE_VB_FMT:
    case AXETAGTYPE_FOCAL_POINT_EVENT_TYPE:
    case AXETAGTYPE_FOCAL_POINT_ATTACK_TYPE:
    case AXETAGTYPE_TILT_TYPE:
    case AXETAGTYPE_CAPTURE_ACTION:
    case AXETAGTYPE_CAPTURE_SEARCH_METHOD:
    case AXETAGTYPE_FADE_GROUP:
    case AXETAGTYPE_ALIGNMENT:
    case AXETAGTYPE_PICK_STYLE:
    case AXETAGTYPE_TEAM:
    case AXETAGTYPE_PROXIMITY_SHAPE:
    case AXETAGTYPE_AUDIOVOLUME_SHAPE:
    case AXETAGTYPE_WEATHERVOLUME_SHAPE:
    case AXETAGTYPE_MONSTER_RARITY:
    case AXETAGTYPE_PORTAL_RESOLVE_METHOD:
    case AXETAGTYPE_POWER_TEMPLATE:
    case AXETAGTYPE_POWER_SPELLFUNC:
    case AXETAGTYPE_POWER_SPECIALDEATH:
    case AXETAGTYPE_POWER_BRAIN_ACTION:
    case AXETAGTYPE_POWER_LOS_CHECK:
    case AXETAGTYPE_POWER_PAYLOAD:
    case AXETAGTYPE_PATH_NODE_TYPE:
    case AXETAGTYPE_BUFF_STACKING_METHOD:
    case AXETAGTYPE_APPEARANCE_FX_FUNCTION:
    case AXETAGTYPE_AUTOASSIGNLOCATION:
    case AXETAGTYPE_PLAYER_CLASS:
    case AXETAGTYPE_LIGHTING_TYPE:
    case AXETAGTYPE_ITEM_LABEL:
    case AXETAGTYPE_SPAWNTRIGGERTYPE:
    case AXETAGTYPE_CRAFTERTYPE:
    case AXETAGTYPE_CONTACT_FRAME_TYPES:
    case AXETAGTYPE_HIRELING_CLASS:
    case AXETAGTYPE_POWER_CUSTOM_TARGET_FUNC:
    case AXETAGTYPE_DAMAGE_TYPE:
    case AXETAGTYPE_DIFFICULTY:
    case AXETAGTYPE_TETHER_TYPE:
    case AXETAGTYPE_DELIVERY_MECHANISM:
    case AXETAGTYPE_WORLD_EDGE_TYPE:
    case AXETAGTYPE_WORLD_SHAPE_TYPE:
    case AXETAGTYPE_WORLD_CHOKEPOINT_TYPE:
    case AXETAGTYPE_WORLD_CHOKEPOINT_DIR_TYPE:
    case AXETAGTYPE_WORLD_LABEL_TYPE:
    case AXETAGTYPE_WORLD_EXIT_DIR_TYPE:
    case AXETAGTYPE_WORLD_SPECIAL_TILE_TYPE:
    case AXETAGTYPE_CHARGE_STEERING_TYPES:
    case AXETAGTYPE_OPENWORLDMODE:
    case AXETAGTYPE_ACT:
    case AXETAGTYPE_GENDER_ALLOW_INVALID:
    case AXETAGTYPE_POWER_HOTBAR_EXCLUSIVE_TYPES:
    case AXETAGTYPE_POWER_SCROLL_BUFF_EXCLUSIVE_TYPES:
    case AXETAGTYPE_TEXTURE_FORMAT:
        ptType = DT_INT;
        break;
    case AXETAGTYPE_GBID_ITEMTYPE:
    case AXETAGTYPE_GBID_ITEM:
    case AXETAGTYPE_GBID_MONSTERAFFIX:
        ptType = DT_GBID;
        break;
    case AXETAGTYPE_ACCEL:
        ptType = DT_ACCEL;
        break;
    case AXETAGTYPE_MASS:
        ptType = DT_MASS;
        break;
    case AXETAGTYPE_ANGLE:
        ptType = DT_ANGLE;
        break;
    case AXETAGTYPE_ANGULARVELOCITY:
        ptType = DT_ANGULARVELOCITY;
        break;
    case AXETAGTYPE_ANGULARACCEL:
        ptType = DT_ANGULARACCEL;
        break;
    case AXETAGTYPE_TICK:
        ptType = DT_TIME;
        break;
    case AXETAGTYPE_VELOCITY:
        ptType = DT_VELOCITY;
        break;
    case AXETAGTYPE_STARTLOC_NAME:
        ptType = DT_STARTLOC_NAME;
        break;
    case AXETAGTYPE_STRING_FORMULA:
        ptType = DT_STRING_FORMULA;
        break;
    case AXETAGTYPE_ACTORATTRIBUTE:
        ptType = DT_ACTOR_ATTRIBUTE;
        break;
    case AXETAGTYPE_ACTORGROUP:
        ptType = DT_ACTOR_GROUP;
        break;
    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
    return ptType;
}
























int32 TagMapGetInt(const CTagMap * pTagMap, uint32 dwTag)
{
    TagVariant variantTagValue;
    if (!TAGMAP_IS_EMPTY(pTagMap) && pTagMap->m_TagMap.Lookup(dwTag, variantTagValue))
        return variantTagValue.m_TagData;












    return TagMapGetDefaultInt(pTagMap, dwTag);
}







void TagMapSetInt(CTagMap * pTagMap, uint32 dwTag, int32 nValue)
{
    TagVariant variantTagValue;
    TagMapRemoveTag(pTagMap, dwTag);
    MapSerializationUtils::PutIntInVariant(variantTagValue, nValue);







    sTagMapSetValue(pTagMap, dwTag, variantTagValue);
}




uint32 TagMapGetDWORD(const CTagMap * pTagMap, uint32 dwTag)
{
    return (uint32)TagMapGetInt(pTagMap, dwTag);
}



















float TagMapGetFloat(const CTagMap * pTagMap, uint32 dwTag)
{
    TagVariant variantTagValue;
    if (!TAGMAP_IS_EMPTY(pTagMap) && pTagMap->m_TagMap.Lookup(dwTag, variantTagValue))
        return MapSerializationUtils::VariantToFloat(variantTagValue);
    return TagMapGetDefaultFloat(pTagMap, dwTag);
}





























StringFormula * TagMapGetFormula(const CTagMap * pTagMap, uint32 dwTag)
{
    TagVariant variantTagValue;





    if (!TAGMAP_IS_EMPTY(pTagMap) && pTagMap->m_TagMap.Lookup(dwTag, variantTagValue))
        return MapSerializationUtils::VariantToStringFormula(variantTagValue);
    return TagMapGetDefaultStringFormula(pTagMap, dwTag);
}







uint8 * TagMapGetBBEFormula(const CTagMap * pTagMap, uint32 dwTag, uint32 & dwSize)
{
    TagVariant variantTagValue;
    StringFormula * ptStringFormula;
    if (!TAGMAP_IS_EMPTY(pTagMap) && pTagMap->m_TagMap.Lookup(dwTag, variantTagValue))
        ptStringFormula = MapSerializationUtils::VariantToStringFormula(variantTagValue);
    else
        ptStringFormula = TagMapGetDefaultStringFormula(pTagMap, dwTag);
    dwSize = ptStringFormula->serFormula.dwSize;
    return ptStringFormula->pbFormula;
}
















LPCSTR TagMapGetFormulaString(const CTagMap * pTagMap, uint32 dwTag)
{
    return TagMapGetFormula(pTagMap, dwTag)->szFormulaString;
}



























































SNO TagMapGetSNO(const CTagMap * pTagMap, uint32 dwTag)
{
    const AxeTagInfo * ptInfo = TagMapGetTagInfo(pTagMap, dwTag);
    if (ptInfo == NULL)
        return SNO_NONE;

    TagVariant variantTagValue;
    if (!TAGMAP_IS_EMPTY(pTagMap) && pTagMap->m_TagMap.Lookup(dwTag, variantTagValue))
        return variantTagValue.m_TagData;
    return TagMapGetDefaultSNO(pTagMap, dwTag);
}













inline void TagMapSetSNO(CTagMap * pTagMap, uint32 dwTag, SNO snoValue)
{
    TagMapSetInt(pTagMap, dwTag, snoValue);
}







GBID TagMapGetGBID(const CTagMap * pTagMap, uint32 dwTag)
{
    TagVariant variantTagValue;
    if (!TAGMAP_IS_EMPTY(pTagMap) && pTagMap->m_TagMap.Lookup(dwTag, variantTagValue))
        return variantTagValue.m_TagData;
    return TagMapGetDefaultGBID(pTagMap, dwTag);
}






















inline int32 TagMapGetDefaultInt(const CTagMap * pTagMap, const uint32 dwTag)
{
    const AxeTagInfo * ptInfo = TagMapGetTagInfo(pTagMap, dwTag);
    return *(const int32 *)ptInfo->lpDefaultVal;
}








inline float TagMapGetDefaultFloat(const CTagMap * pTagMap, const uint32 dwTag)
{
    const AxeTagInfo * ptInfo = TagMapGetTagInfo(pTagMap, dwTag);
    return *(const float *)ptInfo->lpDefaultVal;
}







inline StringFormula * TagMapGetDefaultStringFormula(const CTagMap * pTagMap, const uint32 dwTag)
{


    return TagMapGetDefaultStringFormula(TagMapGetTagInfo(pTagMap, dwTag));
}



inline StringFormula * TagMapGetDefaultStringFormula(const AxeTagInfo * ptInfo)
{
    StringFormulaDefault * pFormulaDefault = (StringFormulaDefault *)ptInfo->lpDefaultVal;



    pFormulaDefault->Compile();
    return &pFormulaDefault->tFormula;
}




SNO TagMapGetDefaultSNO(const CTagMap * pTagMap, const uint32 dwTag)
{
    const AxeTagInfo * ptInfo = TagMapGetTagInfo(pTagMap, dwTag);
    LPCSTR szName = *(const LPCSTR *)ptInfo->lpDefaultVal;
    if (szName[0] != '\0')
    {
        SNOGroup snoGroup = AxeTagTypeToSNOGroup(ptInfo->eType);
        return SNOFileLookupGUID(snoGroup, szName);
    }
    return SNO_NONE;
}














GBID TagMapGetDefaultGBID(const CTagMap * pTagMap, const uint32 dwTag)
{
    const AxeTagInfo * ptInfo = TagMapGetTagInfo(pTagMap, dwTag);
    LPCSTR szName = *(const LPCSTR *)ptInfo->lpDefaultVal;
    if (szName[0] != '\0')
    {
        GameBalanceType eGBType = AxeTagTypeToGBType(ptInfo->eType);
        return GBIDExists(szName, eGBType);
    }
    return (GBID)-1;
}























































static void sTagMapSetValue(CTagMap * pTagMap, uint32 dwTag, TagVariant & variantTagValue)
{
    const AxeTagInfo * ptInfo = TagMapGetTagInfo(pTagMap, dwTag);
    if (ptInfo == NULL)
        return;

    Blizzard::Lock::ScopedMutex scopedMutex(stTagMapPoolCritical);
    pTagMap->m_TagMap.SetAt_Tracked(dwTag, variantTagValue);
}















static inline BOOL sTagMapIsDefaultValue(const CTagMap * pTagMap, uint32 dwTag, TagVariantType eTagVariantType,
                                         TagVariant variantTagValue)
{
    switch (eTagVariantType)
    {
    case TAGVARIANTTYPE_INT:
    case TAGVARIANTTYPE_ACTOR_GROUP:
    case TAGVARIANTTYPE_ACTOR_ATTRIBUTE:
    case TAGVARIANTTYPE_STARTLOC_NAME:



        return variantTagValue.m_TagData == TagMapGetDefaultInt(pTagMap, dwTag);
    case TAGVARIANTTYPE_FLOAT:
        return MapSerializationUtils::VariantToFloat(variantTagValue) == TagMapGetDefaultFloat(pTagMap, dwTag);
    case TAGVARIANTTYPE_SNO:
        return variantTagValue.m_TagData == TagMapGetDefaultSNO(pTagMap, dwTag);
    }
    return FALSE;
}





static void sTagMapSerializeHelper(const CTagMap * pTagMap, void *& pBlockPtr, int32 & nBlockSize, uint32 dwFlags)
{
    int32 nMapCount;
    POS pos;
    BOOL fSkipDefaultValue = SigmaPlatformIsConsole(geSigmaPlatform) && !(dwFlags & 0x20000);

    nMapCount = pTagMap->m_TagMap.Count();
    if (fSkipDefaultValue)
    {
        POS pos = pTagMap->m_TagMap.GetStartAssoc();
        while (pos)
        {
            TagVariantType eTagVariantType;
            uint32 dwTag;
            TagVariant variantTagValue;
            pTagMap->m_TagMap.GetNextAssoc(pos, dwTag, variantTagValue);
            eTagVariantType = TagGetVariantType(pTagMap, dwTag);
            if (sTagMapIsDefaultValue(pTagMap, dwTag, eTagVariantType, variantTagValue))
                nMapCount--;
        }
    }

    MapSerializationUtils::SerializeInt(nMapCount, pBlockPtr, nBlockSize);

    pos = pTagMap->m_TagMap.GetStartAssoc();
    while (pos)
    {
        uint32 dwTag;
        TagVariantType eTagVariantType;
        TagVariant variantTagValue;
        pTagMap->m_TagMap.GetNextAssoc(pos, dwTag, variantTagValue);
        eTagVariantType = TagGetVariantType(pTagMap, dwTag);
        if (!fSkipDefaultValue || !sTagMapIsDefaultValue(pTagMap, dwTag, eTagVariantType, variantTagValue))
        {
            MapSerializationUtils::SerializeInt(eTagVariantType, pBlockPtr, nBlockSize);
            MapSerializationUtils::SerializeInt(dwTag, pBlockPtr, nBlockSize);
            MapSerializationUtils::SerializeVariant(eTagVariantType, variantTagValue, pBlockPtr, nBlockSize);
        }
    }
}
















CTagMap * TagMapRef(CTagMap * pTagMap)
{
    if (TAGMAP_IS_EMPTY(pTagMap))
        return TagMapCreateEmpty((TagBases)((uint32)(uintptr)pTagMap << 16));

    Blizzard::Lock::ScopedMutex scopedMutex(stTagMapPoolCritical);
    pTagMap->m_dwRefCount++;
    return pTagMap;
}















BOOL TagMapCanBeRefed(const CTagMap * pTagMap)
{



    POS pos = TagMapGetStartPosition(pTagMap);
    while (pos)
    {
        uint32 dwTag = TagMapGetNextTag(pTagMap, pos);

        const AxeTagInfo * ptTagInfo = TagMapGetTagInfo(pTagMap, dwTag);
        if (ptTagInfo)
        {
            if (TagGetVariantType(ptTagInfo->eType) == TAGVARIANTTYPE_FORMULA)
                return FALSE;
        }
    }
    return TRUE;
}










BOOL TagMapCompare(const CTagMap * pTagMap1, const CTagMap * pTagMap2)
{
    POS pos;




    if (TagMapGetTagBase(pTagMap1) != TagMapGetTagBase(pTagMap2))
        return FALSE;

    if (TAGMAP_IS_EMPTY(pTagMap1))
        return TAGMAP_IS_EMPTY(pTagMap2);
    if (TAGMAP_IS_EMPTY(pTagMap2))
        return FALSE;






    if (pTagMap1->m_TagMap.Count() != pTagMap2->m_TagMap.Count())
        return FALSE;



    pos = TagMapGetStartPosition(pTagMap1);
    while (pos)
    {
        uint32 dwTag = TagMapGetNextTag(pTagMap1, pos);


        if (!TagMapTagExists(pTagMap2, dwTag))
            return FALSE;



        const AxeTagInfo * ptTagInfo = TagMapGetTagInfo(pTagMap1, dwTag);
        if (ptTagInfo)
        {
            switch (TagGetVariantType(ptTagInfo->eType))
            {
            case TAGVARIANTTYPE_INT:
            case TAGVARIANTTYPE_ACTOR_GROUP:
            case TAGVARIANTTYPE_ACTOR_ATTRIBUTE:
            case TAGVARIANTTYPE_STARTLOC_NAME:
                if (TagMapGetInt(pTagMap1, dwTag) != TagMapGetInt(pTagMap2, dwTag))
                    return FALSE;
                break;

            case TAGVARIANTTYPE_FLOAT:


                if (TagMapGetFloat(pTagMap1, dwTag) != TagMapGetFloat(pTagMap2, dwTag))
                    return FALSE;
                break;

            case TAGVARIANTTYPE_GBID:


                if (TagMapGetGBID(pTagMap1, dwTag) != TagMapGetGBID(pTagMap2, dwTag))
                    return FALSE;
                break;

            case TAGVARIANTTYPE_SNO:


                if (TagMapGetSNO(pTagMap1, dwTag) != TagMapGetSNO(pTagMap2, dwTag))
                    return FALSE;
                break;

            case TAGVARIANTTYPE_FORMULA:


                if (TagMapGetFormulaString(pTagMap1, dwTag) != TagMapGetFormulaString(pTagMap2, dwTag))
                    return FALSE;
                break;
            }
        }
    }
    return TRUE;
}


























































































































inline StringFormula::StringFormula()
{
    serFormulaString.dwOffset = 0;
    serFormulaString.dwSize = 0;
    serFormula.dwOffset = 0;
    serFormula.dwSize = 0;
}
















void StringFormula::Copy(const StringFormula & tSrcStringFormula)
{
    this->Dispose();
    if (tSrcStringFormula.serFormulaString.dwSize)
    {
        szFormulaString = (char *)SigmaMemoryNew(tSrcStringFormula.serFormulaString.dwSize, 4, NULL, FALSE);
        SigmaMemoryCopy(szFormulaString, tSrcStringFormula.szFormulaString, tSrcStringFormula.serFormulaString.dwSize);
    }
    if (tSrcStringFormula.serFormula.dwSize)
    {
        pbFormula = (uint8 *)SigmaMemoryNew(tSrcStringFormula.serFormula.dwSize, 4, NULL, FALSE);
        SigmaMemoryCopy(pbFormula, tSrcStringFormula.pbFormula, tSrcStringFormula.serFormula.dwSize);
    }
    serFormula = tSrcStringFormula.serFormula;
    serFormulaString = tSrcStringFormula.serFormulaString;
}




void StringFormula::Set(LPCSTR szFormula)
{
    this->Dispose();
    serFormulaString.dwSize = strlen(szFormula) + 1;
    szFormulaString = (char *)SigmaMemoryNew(serFormulaString.dwSize, 4, NULL, FALSE);
    strcpy(szFormulaString, szFormula);
}


void StringFormula::Compile()
{
    DisposeFormula();
    pbFormula = (uint8 *)SigmaMemoryNew(1024, 4, NULL, TRUE);
    BBETranslationData bbeTranslationData;
    bbeTranslationData.eGBType = -1;
    bbeTranslationData.gbid = -1;
    uint32 dwFormulaSize = BBEFromString(szFormulaString, pbFormula, 1024, bbeTranslationData);
    serFormula.dwSize = dwFormulaSize;
}







void StringFormula::Dispose()
{
    SigmaMemoryFree(szFormulaString, NULL);
    szFormulaString = NULL;
    serFormulaString.dwSize = 0;
    DisposeFormula();
}

inline void StringFormula::DisposeFormula()
{
    SigmaMemoryFree(pbFormula, NULL);
    pbFormula = NULL;
    serFormula.dwSize = 0;
}



// NON_MATCHING: clang schedules the zero store at +0x40 after the paired zero stores.
StringFormulaDefault::StringFormulaDefault(LPCSTR szFormulaString)
{
    bCompiled = FALSE;
    tFormula.szFormulaString = (char *)szFormulaString;
    tFormula.serFormulaString.dwSize = strlen(szFormulaString) + 1;
    SigmaMemoryClear(arFormula, sizeof(arFormula));
}







inline void StringFormulaDefault::Compile()
{
    if (!bCompiled)
    {
        BBETranslationData bbeTranslationData;
        bbeTranslationData.eGBType = -1;
        bbeTranslationData.gbid = -1;
        uint32 dwFormulaSize = BBEFromString(tFormula.szFormulaString, arFormula, sizeof(arFormula), bbeTranslationData);
        tFormula.pbFormula = arFormula;
        tFormula.serFormula.dwSize = dwFormulaSize;
        bCompiled = TRUE;
    }
}











int32 TagMapGetInheritedValueInt(const CTagMap * ptTagMapDef, const CTagMap * ptTagMapMarker, const uint32 dwActorTag,
                                 const uint32 dwMarkerTag)
{
    if (ptTagMapMarker != NULL && TagMapTagExists(ptTagMapMarker, dwMarkerTag))
        return TagMapGetInt(ptTagMapMarker, dwMarkerTag);
    return TagMapGetInt(ptTagMapDef, dwActorTag);
}














float TagMapGetInheritedValueFloat(const CTagMap * ptTagMapDef, const CTagMap * ptTagMapMarker,
                                   const uint32 dwActorTag, const uint32 dwMarkerTag)
{
    if (ptTagMapMarker != NULL && TagMapTagExists(ptTagMapMarker, dwMarkerTag))
        return TagMapGetFloat(ptTagMapMarker, dwMarkerTag);
    return TagMapGetFloat(ptTagMapDef, dwActorTag);
}
