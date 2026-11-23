// X/Client/CActorCommonData.cpp
//
// Client-side ActorCommonData helpers. Byte-matched against Diablo III 2.6.2
// (Switch, clang 5.0.1). The line attributions in the comments come from
// build/lines/X/Client/CActorCommonData.cpp and the matching
// build/dwarf/X/Client/CActorCommonData.cpp records:
//
//   :20    0x7100346390  CACDSetAffix                        (120 bytes)
//   :39    0x7100346410  CACDSetRareMonsterNamesAndAffixes   ( 84 bytes)
//   :58    0x7100346470  CACDAttributeSetValue               (164 bytes)
//   :81    0x7100346520  ACDUpdateAppearance                 (204 bytes)
//   :116   0x71003465f0  ACDReapplyVisualInventory           (288 bytes)
//
// ACDTryToGet/ACDGetOwner are the inline helpers owned by X/Common/ActorCommonData.h;
// RActorTryToGet by X/Client/RActors/RActors.h; AddTail_Tracked by
// X/Sigma/DataStructures/XPooledList.h. Their inlined copies (including the TLS /
// GameCommonData / TChainedDataArray chains listed in the DWARF) come from those
// headers, not from this file.

#include "CPlayer.h"
#include "RActors/RActors.h"
#include "../Common/ActorCommonData.h"
#include "../Common/ActorInventory.h"
#include "../Common/Attributes/FastAttribute.h"
#include "../Common/VisualInventory.h"

// _Z31VisualInventoryClearAttachmentsP6RActor is defined in X/Client/CVisualInventory.cpp;
// that translation unit has no adopted header in this checkout, so the entry point this
// file calls is declared here.
void VisualInventoryClearAttachments(RActor* ptRActor);

// CActorCommonData.cpp:20 -- 0x7100346390 (120 bytes).
//   w19 = gbidAffix; cmn w19,#1 -> early-out.
//   add x8,x0,#0xe8 / add x9,x0,#0x110 / cmp w2,#0 / csel x20,x9,x8,eq is the address
//   select: condition true (fIdentified != 0) -> x20 = identified list at +0xe8, else
//   unidentified list at +0x110. The select stays on the address, then the list insert
//   (XPooledList.h:179 -> XBaseList.h:606 -> XChainedFixedBlockAllocator.h:895).
void CACDSetAffix(ActorCommonData& tACD, const GBID gbidAffix, const BOOL fIdentified)
{
    if (gbidAffix == -1)
        return;

    GBIDPooledList& arAffixes = fIdentified ? tACD.listIdentifiedAffixGBIDs
                                            : tACD.listUnidentifiedAffixGBIDs;

    arAffixes.AddTail_Tracked(gbidAffix);   // XPooledList.h:179 -> XBaseList.h:606
}

// CActorCommonData.cpp:39 -- 0x7100346410 (84 bytes).
//   pRareNameGBIDs[0..1] at +0x170, pMonsterAffixGBIDs[0..7] at +0x178; both fixed
//   counts fully unrolled (DWARF shows one local int32 i per block at :46 and :50).
void CACDSetRareMonsterNamesAndAffixes(ActorCommonData& tACD,
                                       const GBID* pgbidRareNames,
                                       const GBID* pgbidMonsterAffixes)
{
    for (int32 i = 0; i < 2; ++i)
        tACD.pRareNameGBIDs[i] = pgbidRareNames[i];

    for (int32 i = 0; i < 8; ++i)
        tACD.pMonsterAffixGBIDs[i] = pgbidMonsterAffixes[i];
}

// CActorCommonData.cpp:58 -- 0x7100346470 (164 bytes).
//   FastAttribValue is non-trivial for calls: the by-value tValue arrives as a pointer
//   in x2 (x21 here), FastAttribGetValue returns through the sret slot at sp+0x18, and
//   both FastAttribLightSetValue's tValue argument and the RActorsACDAttributeChanged
//   tLastValue/tValue arguments are copied into caller temporaries.
//   nDifference is memcmp, not !=.  DWARF types the local as int32 and its register
//   range starts at 0x3464cc, immediately after `cset w24,ne` at 0x3464c8 and before
//   `bl FastAttribLightSetValue` at 0x3464cc: the int32 is formed before the call and
//   tested with `cbz w24` after.  A memcmp call keeps the int32 result across the call
//   (clang 5 cannot fold it: tValue has no known alignment), matching the original.
void CACDAttributeSetValue(ActorCommonData& tACD, FastAttribKey tKey, FastAttribValue tValue,
                           const BOOL fBypassRActorChanges)
{
    AttribGroupID idFastAttrib = tACD.idFastAttrib;                        // F0:64

    FastAttribValue tLastValue = FastAttribGetValue(idFastAttrib, tKey);   // F0:65

    int32 nDifference = memcmp(&tLastValue, &tValue, sizeof(FastAttribValue));  // F0:67

    FastAttribLightSetValue(idFastAttrib, tKey, tValue);                   // F0:69

    if (!fBypassRActorChanges && nDifference)                              // F0:71
        RActorsACDAttributeChanged(tACD, tKey, tLastValue, tValue);        // F0:74
}

// CActorCommonData.cpp:81 -- 0x7100346520 (204 bytes).
//   The invalid-id early-out at F1:1448 belongs to the inlined ACDTryToGet. The switch
//   is lowered as the (type-7)<=1 range test first, then type==1 (both tail calls).
//   The trailing `ret` at F0:111 is the switch fall-through, not a default body.
void ACDUpdateAppearance(ACDID idACD)
{
    ActorCommonData* ptACD = ACDTryToGet(idACD);                          // F0:86
    if (!ptACD)
        return;

    InventoryType eInventoryType = ACDInventoryGetType(*ptACD);           // F0:90

    switch (eInventoryType)                                               // F0:97
    {
    case INVENTORY_PLAYER:                                                // 1
        CPlayerAppearanceUpdate(idACD);                                   // F0:100
        break;

    case INVENTORY_HIRELING:                                              // 7
    case INVENTORY_GENERIC:                                               // 8
        CPlayerPetAppearanceUpdate(idACD);                                // F0:105
        break;
    }
}                                                                         // F0:111

// CActorCommonData.cpp:116 -- 0x71003465f0 (288 bytes).
//   All failure paths of the inlined ACDGetOwner/RActorTryToGet funnel past the
//   `if (ptRActor)` body to the VisualInventoryGet at F0:129, so the body is only the
//   single VisualInventoryClearAttachments call at F0:124.
void ACDReapplyVisualInventory(ACDID idACD)
{
    RActorID idRActor = (RActorID)ACDGetOwner(idACD);                     // F0:120
    RActor* ptRActor = RActorTryToGet(idRActor);                          // F0:121

    if (ptRActor)
        VisualInventoryClearAttachments(ptRActor);                        // F0:124

    VisualInventory* ptVisualInventory = VisualInventoryGet(idACD);       // F0:129
    if (ptVisualInventory)
        ptVisualInventory->InitializeLast();                              // F0:132

    ACDUpdateAppearance(idACD);                                           // F0:135
}
