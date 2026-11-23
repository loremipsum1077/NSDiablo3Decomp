// Original file: Shared/Shared/Version/Version.cpp
// Only the functions that survive in the image are written; the rest of the file was removed by
// the linker.
#include "Version.h"

namespace Version {
// line 168
const Quad Quad::k_invalid;

// line 172
inline Quad::Quad() : stringForm(NULL)
{
    Clear(k_invalidValue);
}

// line 178
Quad::~Quad()
{
    delete stringForm;
}

// line 234
inline void Quad::Clear(int clearTo)
{
    for (int i = 0; i < k_size; i++)
    {
        m_values[i] = clearTo;
    }
}

// line 852
const ComponentLocator ComponentLocator::k_invalid;

// line 856
inline ComponentLocator::ComponentLocator() : m_interpreter(e_Invalid), m_interpreterBlob(NULL)
{
}

// line 863
ComponentLocator * ComponentLocator::Clone() const
{
    return new ComponentLocator(this);
}

// line 869
bool ComponentLocator::GetLocation(Blizzard::STL::string &)
{
    return false;
}

// line 891
inline ComponentLocator::ComponentLocator(const ComponentLocator * other)
    : m_interpreter(other->m_interpreter), m_interpreterBlob(other->m_interpreterBlob)
{
}

// line 906
bool ComponentLocator::Locate(Components & foundComponents)
{
    return false;
}

// line 1697
const Blizzard::STL::string Versioner::s_product[4] = { "WoW", "S2", "S2", "D3" };
}  // namespace Version
