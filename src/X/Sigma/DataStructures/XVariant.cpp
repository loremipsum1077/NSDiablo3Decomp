#include "XVariant.h"
#include <new>
#include <utility>













XVariant::XVariant()
    : m_type(VARIANT_NONE) {
}




XVariant::XVariant(const XVariant& v)
    : m_type(VARIANT_NONE) {

    Copy(v);
}



XVariant::XVariant(XVariant&& v)
    : m_type(VARIANT_NONE) {

    Move(std::move(v));
}



XVariant& XVariant::operator=(const XVariant& v) {

    Clear();
    Copy(v);
    return *this;
}



XVariant& XVariant::operator=(XVariant&& v) {

    Clear();
    Move(std::move(v));
    return *this;
}



XVariant::~XVariant() {

    Clear();
}



XVariant& XVariant::SetBool(bool value) {

    Clear();
    m_type = VARIANT_BOOL;
    m_value.m_bool = value;
    return *this;
}





























XVariant& XVariant::SetInt32(int32 value) {

    Clear();
    m_type = VARIANT_INT32;
    m_value.m_int = value;
    return *this;
}






































XVariant& XVariant::SetInt64(int64 value) {

    Clear();
    m_type = VARIANT_INT64;
    m_value.m_int = value;
    return *this;
}






































XVariant& XVariant::SetUint32(uint32 value) {

    Clear();
    m_type = VARIANT_UINT32;
    m_value.m_uint = value;
    return *this;
}





































XVariant& XVariant::SetUint64(uint64 value) {

    Clear();
    m_type = VARIANT_UINT64;
    m_value.m_uint = value;
    return *this;
}



bool XVariant::GetUint64(uint64& value) const {
    if (m_type != VARIANT_UINT64)
        return false;

    value = m_value.m_uint;
    return true;
}






uint64 XVariant::AsUint64() const {
    return m_value.m_uint;
}










































































































XVariant& XVariant::SetString(const blz::string& value) {

    Clear();
    m_type = VARIANT_STRING;
    new (m_value.m_data) blz::string(value.c_str());
    return *this;
}






















































// Inlined everywhere; needs blz::basic_string::~basic_string.
void XVariant::Clear() {
    switch (m_type) {
    case VARIANT_STRING:
    case VARIANT_BLOB:

        ((blz::string*)m_value.m_data)->~basic_string();
        break;
    default:
        break;
    }
    m_type = VARIANT_NONE;
}














void XVariant::Copy(const XVariant& v) {
    m_type = v.m_type;
    switch (m_type) {
    case VARIANT_NONE:
        break;
    case VARIANT_BOOL:
        m_value.m_bool = v.m_value.m_bool;
        break;
    case VARIANT_UINT32:
    case VARIANT_UINT64:
        m_value.m_uint = v.m_value.m_uint;
        break;
    case VARIANT_INT32:
    case VARIANT_INT64:
        m_value.m_int = v.m_value.m_int;
        break;
    case VARIANT_FLOAT:
    case VARIANT_DOUBLE:
        m_value.m_float = v.m_value.m_float;
        break;
    case VARIANT_STRING:
    case VARIANT_BLOB:









        new (m_value.m_data) blz::string(*(const blz::string*)v.m_value.m_data);
        break;
    }
}



void XVariant::Move(XVariant&& v) {
    m_type = v.m_type;
    switch (m_type) {
    case VARIANT_NONE:
        break;
    case VARIANT_BOOL:
        m_value.m_bool = v.m_value.m_bool;
        break;
    case VARIANT_UINT32:
    case VARIANT_UINT64:
        m_value.m_uint = v.m_value.m_uint;
        break;
    case VARIANT_INT32:
    case VARIANT_INT64:
        m_value.m_int = v.m_value.m_int;
        break;
    case VARIANT_FLOAT:
    case VARIANT_DOUBLE:
        m_value.m_float = v.m_value.m_float;
        break;
    case VARIANT_STRING:
        new (m_value.m_data) blz::string(std::move(v.GetData()));
        break;
    case VARIANT_BLOB:







        new (m_value.m_data) blz::string(std::move(v.GetData()));
        break;
    }
}
