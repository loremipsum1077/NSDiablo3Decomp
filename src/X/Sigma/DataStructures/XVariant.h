#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/string.h"

class XVariant {
    enum Type {
        VARIANT_NONE = 0,
        VARIANT_BOOL = 1,
        VARIANT_UINT32 = 2,
        VARIANT_UINT64 = 3,
        VARIANT_INT32 = 4,
        VARIANT_INT64 = 5,
        VARIANT_FLOAT = 6,
        VARIANT_DOUBLE = 7,
        VARIANT_STRING = 8,
        VARIANT_BLOB = 9,
    };

public:
    XVariant();
    XVariant(const XVariant& v);
    XVariant(XVariant&& v);




















    static XVariant MakeBool(bool value) { return XVariant().SetBool(value); }
    static XVariant MakeInt32(int32 value) { return XVariant().SetInt32(value); }
    static XVariant MakeInt64(int64 value) { return XVariant().SetInt64(value); }
    static XVariant MakeUint32(uint32 value) { return XVariant().SetUint32(value); }
    static XVariant MakeUint64(uint64 value) { return XVariant().SetUint64(value); }


    static XVariant MakeString(const blz::string& value) { return XVariant().SetString(value); }

    ~XVariant();




    Type GetType() const { return m_type; }

    XVariant& operator=(const XVariant& v);
    XVariant& operator=(XVariant&& v);

    XVariant& SetBool(bool value);
    XVariant& SetInt32(int32 value);
    XVariant& SetInt64(int64 value);
    XVariant& SetUint32(uint32 value);
    XVariant& SetUint64(uint64 value);
    bool GetUint64(uint64& value) const;
    uint64 AsUint64() const;
    XVariant& SetString(const blz::string& value);

    // Declared in the original but not in the binary (bodies unknown):
    // MakeFloat, MakeDouble, MakeBlob, None, SetNone, IsNone, GetBool, AsBool,
    // GetInt32, AsInt32, AsInt32Default, GetInt64, AsInt64, AsInt64Default,
    // GetUint32, AsUint32, AsUint32Default, AsUint64Default, SetFloat,
    // GetFloat, AsFloat, AsFloatDefault, SetDouble, GetDouble, AsDouble,
    // AsDoubleDefault, GetString, AsString, SetBlob, GetBlob, AsBlob, ToData.

private:



































    const blz::string& GetData() const { return *(const blz::string*)m_value.m_data; }
    blz::string& GetData() { return *(blz::string*)m_value.m_data; }
    void Clear();
    void Copy(const XVariant& v);
    void Move(XVariant&& v);

    Type m_type;
    union Value {
        bool m_bool;
        int64 m_int;
        uint64 m_uint;
        double m_float;
        char m_data[40];
    };
    Value m_value;
};

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(XVariant) == 0x30, "XVariant");
