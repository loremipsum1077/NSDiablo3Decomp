// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/UIDialogDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../UserInterface/UIInit.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/Unicode.h"
#include <stddef.h>
#include <stdint.h>

struct FileReference;
struct UIControlData;
struct ImageTextureFrame;
class XMemoryAllocatorInterface;

typedef int32 UIType;

typedef int32 UIAttributeType;

struct UIControlElement {  // size 0x18
    uint64 nKey;  // 0x00
    SerializeData serAttrib_Console;  // 0x08
    XPtr64<char> szAttrib;  // 0x10
};
static_assert(sizeof(UIControlElement) == 0x18, "UIControlElement");

struct UIDataElementInt32 {  // size 0x10
    uint64 nKey;  // 0x00
    int32 nValue;  // 0x08
};
static_assert(sizeof(UIDataElementInt32) == 0x10, "UIDataElementInt32");

struct UIDataElementFloat32 {  // size 0x10
    uint64 nKey;  // 0x00
    float flValue;  // 0x08
};
static_assert(sizeof(UIDataElementFloat32) == 0x10, "UIDataElementFloat32");

typedef blz::vector<UIControlData *, blz::allocator<UIControlData *> > UIControlDataArray;

struct DataElements {  // size 0x48
    uint32 dwNumStringElements;  // 0x00
    SerializeData serStringElements;  // 0x04
    XPtr64<UIControlElement> ptStringElements;  // 0x10
    uint32 dwNumInt32Elements;  // 0x18
    SerializeData serInt32Elements;  // 0x1c
    XPtr64<UIDataElementInt32> ptInt32Elements;  // 0x28
    uint32 dwNumFloat32Elements;  // 0x30
    SerializeData serFloat32Elements;  // 0x34
    XPtr64<UIDataElementFloat32> ptFloat32Elements;  // 0x40
};
static_assert(sizeof(DataElements) == 0x48, "DataElements");

typedef int32 UIAnimationType;

struct UIAnimationDescription {  // size 0x28
    uint64 nKey;  // 0x00
    SerializeData serName_Console;  // 0x08
    XPtr64<char> szName;  // 0x10
    UIAnimationType eType;  // 0x18
    int32 nNodeRange[2];  // 0x1c
};
static_assert(sizeof(UIAnimationDescription) == 0x28, "UIAnimationDescription");

typedef int32 UIAnimationProperty;

struct UIControlAnimationBinding {  // size 0x18
    SerializeData serAnimationName_Console;  // 0x00
    XPtr64<char> szAnimationName;  // 0x08
    UIAnimationProperty eProperty;  // 0x10
    float flScaleFactor;  // 0x14
    UIControlAnimationBinding();
};
static_assert(sizeof(UIControlAnimationBinding) == 0x18, "UIControlAnimationBinding");

struct UIAnimationElementInt32 {  // size 0x8
    float flT;  // 0x00
    int32 nValue;  // 0x04
};
static_assert(sizeof(UIAnimationElementInt32) == 0x8, "UIAnimationElementInt32");

struct UIAnimationElementFloat {  // size 0x8
    float flT;  // 0x00
    float flValue;  // 0x04
};
static_assert(sizeof(UIAnimationElementFloat) == 0x8, "UIAnimationElementFloat");

struct UIAnimationElementVector2D {  // size 0xc
    float flT;  // 0x00
    float flVector[2];  // 0x04
};
static_assert(sizeof(UIAnimationElementVector2D) == 0xc, "UIAnimationElementVector2D");

struct UIAnimationElements {  // size 0x78
    uint32 dwNumInt32Elements;  // 0x00
    SerializeData serInt32Elements;  // 0x04
    XPtr64<UIAnimationElementInt32> ptInt32Elements;  // 0x10
    uint32 dwNumFloatElements;  // 0x18
    SerializeData serFloatElements;  // 0x1c
    XPtr64<UIAnimationElementFloat> ptFloatElements;  // 0x28
    uint32 dwNumVector2DElements;  // 0x30
    SerializeData serVector2DElements;  // 0x34
    XPtr64<UIAnimationElementVector2D> ptVector2DElements;  // 0x40
    uint32 dwNumAnimationDescriptions;  // 0x48
    SerializeData serAnimationDescriptions;  // 0x4c
    XPtr64<UIAnimationDescription> ptAnimationDescriptions;  // 0x58
    uint32 dwNumAnimationBindings;  // 0x60
    SerializeData serAnimationBindings;  // 0x64
    XPtr64<UIControlAnimationBinding> ptAnimationBindings;  // 0x70
};
static_assert(sizeof(UIAnimationElements) == 0x78, "UIAnimationElements");

struct UIStyleChildSpec {  // size 0x70
    UIStyleChildSpec * Clone() const;
    static void DeleteClone(UIStyleChildSpec * pSpec, const BOOL fAllocatedDuringCompilation);
    SerializeData serPathToChild_Console;  // 0x00
    XPtr64<char> szPathToChild;  // 0x08
    DataElements tAttributes;  // 0x10
    XPtr64<XGrowableMap<unsigned long, unsigned long, UIControlElement *, UIControlElement *> > pmapControlElements;  // 0x58
    XPtr64<XGrowableMap<unsigned long, unsigned long, UIDataElementInt32 *, UIDataElementInt32 *> > pmapInt32Elements;  // 0x60
    XPtr64<XGrowableMap<unsigned long, unsigned long, UIDataElementFloat32 *, UIDataElementFloat32 *> > pmapFloat32Elements;  // 0x68
};
static_assert(sizeof(UIStyleChildSpec) == 0x70, "UIStyleChildSpec");

struct UIStyle {  // size 0xa0
    UIStyle * Clone() const;
    void CopyTo(UIStyle * pDest) const;
    static void DeleteClone(UIStyle * pStyle, const BOOL fAllocatedDuringCompilation);
    BOOL GetAttribute(const UIAttributeType eAttributeType, UIControlElement *& pControlElement) const;
    BOOL GetAttribute(const UIAttributeType eAttributeType, UIDataElementInt32 *& pDataElement) const;
    BOOL GetAttribute(const UIAttributeType eAttributeType, UIDataElementFloat32 *& pDataElement) const;
    BOOL GetImageAttribute(const UIAttributeType, ImageTextureFrame &) const;
    uint64 nKey;  // 0x00
    SerializeData serName_Console;  // 0x08
    XPtr64<char> szName;  // 0x10
    DataElements tAttributes;  // 0x18
    uint32 dwNumChildSpecs;  // 0x60
    SerializeData serChildSpecs;  // 0x64
    XPtr64<UIStyleChildSpec> ptChildSpecs;  // 0x70
    XPtr64<XGrowableMap<unsigned long, unsigned long, UIControlElement *, UIControlElement *> > pmapControlElements;  // 0x78
    XPtr64<XGrowableMap<unsigned long, unsigned long, UIDataElementInt32 *, UIDataElementInt32 *> > pmapInt32Elements;  // 0x80
    XPtr64<XGrowableMap<unsigned long, unsigned long, UIDataElementFloat32 *, UIDataElementFloat32 *> > pmapFloat32Elements;  // 0x88
    XPtr64<blz::vector<UIStyleChildSpec *, blz::allocator<UIStyleChildSpec *> > > plistChildSpecs;  // 0x90
    XPtr64<UIStyle> pBaseStyle;  // 0x98
};
static_assert(sizeof(UIStyle) == 0xa0, "UIStyle");

typedef XGrowableMap<unsigned long, unsigned long, UIStyle *, UIStyle *> UIStyleMap;

struct UIControlData {  // size 0x170
    BOOL AttributeExists(const UIAttributeType eAttributeType) const;
    LPCSTR GetAttributeValue(const UIAttributeType eAttributeType) const;
    int32 GetInt32Value(const UIAttributeType eAttributeType) const;
    float GetFloat32Value(const UIAttributeType eAttributeType) const;
    BOOL GetBoolValue(const UIAttributeType eAttributeType) const;
    uint32 GetUint32Value(const UIAttributeType eAttributeType) const;
    BOOL GetImageAttribute(const UIAttributeType eAttributeType, ImageTextureFrame & tImageTexFrame) const;
    UIControlData * Clone() const;
    static UIControlData * Create(UIType, LPCSTR, LPCSTR);
    static void Delete(UIControlData * pClone, const BOOL fAllocatedDuringCompilation);
    int32 nOrdinal;  // 0x00
    uint64 nKey;  // 0x08
    SerializeData serName_Console;  // 0x10
    XPtr64<char> szName;  // 0x18
    uint64 nParentKey;  // 0x20
    SerializeData serParentKey_Console;  // 0x28
    XPtr64<char> szParentKey;  // 0x30
    UIType eControlType;  // 0x38
    UIStyle tInlineStyle;  // 0x40
    UIAnimationElements tAnimationElements;  // 0xe0
    XPtr64<blz::vector<UIControlData *, blz::allocator<UIControlData *> > > plistChildren;  // 0x158
    XPtr64<UIStyle> pBaseStyle;  // 0x160
    XPtr64<blz::vector<UIStyleChildSpec *, blz::allocator<UIStyleChildSpec *> > > plistStylesFromAncestors;  // 0x168
};
static_assert(sizeof(UIControlData) == 0x170, "UIControlData");

struct UIControlDataExt {  // size 0x30
    // vtable pointer
    UIControlDataExt(UIControlData &);
    virtual ~UIControlDataExt() {  // vtable slot 0; line 303
        ::delete m_template;
        m_template = NULL;
    }
    UIControlDataExt * GetTemplate() const { return m_template; }  // line 308
    SNO GetTemplateSno() const { return m_templateSno; }  // line 309
    void SetTemplate(UIControlData * pTemplate, SNO templateSno);
    void GetControlChildren(UIControlDataArray * plistChildren) const;
    void GetAllChildren(UIControlDataArray * plistChildren) const;
    void CloneTemplateChildren(UIControlDataArray * plistChildren) const;
    BOOL AttributeExists(const UIAttributeType eAttributeType) const;
    LPCSTR GetAttributeValue(const UIAttributeType eAttributeType) const;
    int32 GetInt32Value(const UIAttributeType eAttributeType) const;
    float GetFloat32Value(const UIAttributeType eAttributeType) const;
    BOOL GetBoolValue(const UIAttributeType eAttributeType) const;
    uint32 GetUint32Value(const UIAttributeType eAttributeType) const;
    BOOL GetImageAttribute(const UIAttributeType eAttributeType, ImageTextureFrame & tImageTexFrame) const;
    uint64 nKey;  // 0x08
    UIType eControlType;  // 0x10
    UIControlData & m_control;  // 0x18
private:
    UIControlDataExt * m_template;  // 0x20
    SNO m_templateSno;  // 0x28
};
static_assert(sizeof(UIControlDataExt) == 0x30, "UIControlDataExt");

struct DataMaps {  // size 0x18
    XPtr64<XGrowableMap<unsigned long, unsigned long, CRefString, CRefString> > pmapStrings;  // 0x00
    XPtr64<XGrowableMap<unsigned long, unsigned long, int, int> > pmapInt32s;  // 0x08
    XPtr64<XGrowableMap<unsigned long, unsigned long, float, float> > pmapFloat32s;  // 0x10
};
static_assert(sizeof(DataMaps) == 0x18, "DataMaps");

struct UIDialogDefinition {  // size 0xb8
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwMachineTime;  // 0x0c
    uint32 dwNumControls;  // 0x10
    SerializeData serControlList;  // 0x14
    XPtr64<UIControlData> ptControlList;  // 0x20
    uint32 dwNumStyles;  // 0x28
    SerializeData serStyleList;  // 0x2c
    XPtr64<UIStyle> ptStyleList;  // 0x38
    DataElements tConstants;  // 0x40
    XPtr64<XGrowableMap<unsigned long, unsigned long, UIControlData *, UIControlData *> > pmapControlData;  // 0x88
    XPtr64<XGrowableMap<unsigned long, unsigned long, UIStyle *, UIStyle *> > pmapStyles;  // 0x90
    DataMaps tConstantsMaps;  // 0x98
    XPtr64<UIControlData> pControlDataRoot;  // 0xb0
};
static_assert(sizeof(UIDialogDefinition) == 0xb8, "UIDialogDefinition");

class UIDialogGroupDef : public GroupDefinition {  // size 0xa0
public:
    UIDialogGroupDef();
    virtual ~UIDialogGroupDef() {}  // line 386
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 391; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 394
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 396
        return SNOPACKTYPE_CLIENTDATA;
    }
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual void Delete(void * lpDefinition) const;  // vtable slot 18
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
};
static_assert(sizeof(UIDialogGroupDef) == 0xa0, "UIDialogGroupDef");

extern UIDialogGroupDef * gptGroupDefUI;  // UIDialogDefinition.cpp:534

class XTypeDescriptor;
extern const XTypeDescriptor * DT_UI;  // UIDialogDefinition.cpp:527

typedef ScopedDefinitionLockR<UIDialogDefinition> ScopedUIDialogDefinitionLockR;

template <> inline const UIDialogDefinition * DefinitionLockR<UIDialogDefinition>(SNO sno) {  // line 444
    return (const UIDialogDefinition *)gptGroupDefUI->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<UIDialogDefinition>(const UIDialogDefinition * pdef) {  // line 445
    void * lpDef = (void *)pdef;
    gptGroupDefUI->UnlockR(lpDef);
}


