// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Shared/Shared/Version/Version.h
// Only the classes Version.cpp's surviving functions need are adopted so far.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../BlizzardCore/Include/Blizzard/Util/STL.h"
#include <string>
#include <vector>

namespace Version {
class Quad {  // size 0x18
protected:
    static const int k_invalidValue = -1;
    static const int k_size = 4;
    int m_values[4];  // 0x00
    static const Version::Quad k_invalid;
    std::string * stringForm;  // 0x10

public:
    Quad();
    Quad(int, int, int, int);
    Quad(int *);
    Quad(const Version::Quad &);
    ~Quad();
    void Clear(int clearTo);
};
static_assert(sizeof(Quad) == 0x18, "Quad");

enum Interpreter {
    e_Invalid = 0,
    e_DoNotDecode = 1,
    e_EXE = 2,
    e_EXEWithoutBlizzardComponent = 3,
    e_APP = 4,
    e_APPWithoutBlizzardComponent = 5,
    e_MPQ = 6,
    e_DLL = 7,
    e_LibraryBundle = 8,
    e_BattlenetBundle = 9,
    e_ShaderCache = 10,
    e_InterpreterLast = 11,
    e_InterpreterFirst = 0,
    e_InterpreterFirstDecodable = 2,
};

class Component;
typedef std::vector<Version::Component> Components;

class ComponentLocator {  // size 0x18
protected:
    Version::Interpreter m_interpreter;  // 0x08
    void * m_interpreterBlob;  // 0x10
    virtual bool GetLocation(Blizzard::STL::string &);  // vtable slot 0

public:
    static const Version::ComponentLocator k_invalid;
    ComponentLocator();
    ComponentLocator(const Version::Interpreter, void *);
    ComponentLocator(const Version::ComponentLocator &);
    ComponentLocator(const Version::ComponentLocator *);
    virtual ~ComponentLocator() {}
    virtual Version::ComponentLocator * Clone() const;  // vtable slot 3
    Version::Interpreter InterpretAs() const;
    void * InterpretWith() const;
    virtual bool Valid() const { return m_interpreter != e_DoNotDecode; }  // vtable slot 4
    virtual bool Locate(Version::Components & foundComponents);  // vtable slot 5
};
static_assert(sizeof(ComponentLocator) == 0x18, "ComponentLocator");

class Versioner {
public:
    enum Product {
        WOW = 0,
        SC2 = 1,
        SC2PTR = 2,
        D3 = 3,
        PRODUCT_MAX = 4,
    };

protected:
    static const Blizzard::STL::string s_product[4];
};

}  // namespace Version
