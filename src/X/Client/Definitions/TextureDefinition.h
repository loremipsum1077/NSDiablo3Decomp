// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/TextureDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Gfx/GfxCommon.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/Math/Rect.h"

struct FileReference;

enum {
    TEXTURE_MAX_FACES = 6,
    TEXTURE_MAX_WIDTH = 2048,
    MAX_MIPMAP_LEVELS = 10,
};

enum {
    TEXTURE_DEF_UNMANAGED_BIT = 0,
    TEXTURE_DEF_IGNORE_MIPOFFSET_BIT = 1,
    TEXTURE_DEF_IMPORT_FLAGS_VALID_BIT = 2,
};

struct ImageFileID {  // size 0x4
    int32 nFrameIndex;  // 0x00
};
static_assert(sizeof(ImageFileID) == 0x4, "ImageFileID");

struct TextureDefinition {  // size 0x278
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwRuntimeFlags;  // 0x0c
    TextureFormat eTexFormat;  // 0x10
    uint32 dwWidth;  // 0x14
    uint32 dwHeight;  // 0x18
    uint32 dwFaceCount;  // 0x1c
    uint32 dwDepth;  // 0x20
    uint32 dwPadding2;  // 0x24
    uint32 dwMipMapLevelMin;  // 0x28
    uint32 dwMipMapLevelMax;  // 0x2c
    SerializeData serTex[6][10];  // 0x30
    uint32 dwFrameCount;  // 0x210
    SerializeData serFrame;  // 0x214
    XPtr64<TexFrame> ptFrame;  // 0x220
    IVector2D pHotspot;  // 0x228
    XPtr64<IRect2D> prectFrame;  // 0x230
    TexData tTexData;  // 0x238
    ResourceUsage eUsage;  // 0x240
    uint32 dwLastFrameChecked;  // 0x244
    XPtr64<ImageFileID> ptImageFileIDs;  // 0x248
    SerializeData serImageFileIDs;  // 0x250
    BOOL bCubemap;  // 0x258
    uint32 dwImportFlags;  // 0x25c
    uint32 dwMachineTime;  // 0x260
    uint32 dwConsoleMipOffset;  // 0x264
    float flConsoleLightmapVScale;  // 0x268
    uint32 dwVolumeXSlices;  // 0x26c
    uint32 dwVolumeYSlices;  // 0x270
};
static_assert(sizeof(TextureDefinition) == 0x278, "TextureDefinition");

class TextureGroupDef : public GroupDefinition {  // size 0xa0
public:
    TextureGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return FALSE; }  // vtable slot 13, line 164
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 166
        return SNOPACKTYPE_TEXTURE;
    }
    virtual void Read(const RequiredDefinitionHeader & tHeader, void *& lpDefinition, SNOElement * ptSNOElement, BOOL fContiguousBuffer, uint32 dwContiguousSize, XMemoryAllocatorInterface * ptMemoryInterface) const;  // vtable slot 14
    virtual void Dispose();  // vtable slot 10
    virtual void Delete(void * lpDefinition) const;  // vtable slot 18
    void PurgeAllDynamic();
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual uint32 GetContiguousReadSize(SNOElement * ptSNOElement);  // vtable slot 27
};
static_assert(sizeof(TextureGroupDef) == 0xa0, "TextureGroupDef");

extern TextureGroupDef * gptGroupDefTexture;  // TextureDefinition.cpp:266

typedef ScopedDefinitionLockR<TextureDefinition> ScopedTextureDefinitionLockR;

template <> inline const TextureDefinition * DefinitionLockR<TextureDefinition>(SNO sno) {  // line 255
    return (const TextureDefinition *)gptGroupDefTexture->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<TextureDefinition>(const TextureDefinition * pdef) {  // line 256
    void * lpDef = (void *)pdef;
    gptGroupDefTexture->UnlockR(lpDef);
}
template <> inline TextureDefinition * DefinitionLockW<TextureDefinition>(SNO sno) {  // line 257
    return (TextureDefinition *)gptGroupDefTexture->LockW(sno, FALSE);
}
template <> inline void DefinitionUnlockW<TextureDefinition>(TextureDefinition * pdef) {  // line 258
    void * lpDef = pdef;
    gptGroupDefTexture->UnlockW(lpDef);
}

typedef ScopedDefinitionLockW<TextureDefinition> ScopedTextureDefinitionLockW;

// TextureDefinition.cpp:994
SNO TextureDefinitionCreateTexture(LPCSTR szTexDebugName, const TextureFormat eDesiredTexFormat, TextureFormat & eSupportedTextureFormat,
                                   const uint32 dwWidth, const uint32 dwHeight, const ResourceUsage eUsage, const uint32 dwTexCreateFlags,
                                   const uint32 dwMSAALevel, const uint32 dwTargetERAMOffset, const int32 nExponentBias);
// TextureDefinition.cpp:2776
void TextureComputeFramePixelCoords(TextureDefinition * pdefTexture);
// TextureDefinition.cpp:268
void TextureDefinitionSetTexture(const SNO sno, TexData & tTexData);
