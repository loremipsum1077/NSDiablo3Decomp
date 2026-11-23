// Original file: X/Sigma/Files/XFilesCommon.cpp

#include "XFiles.h"
#include "XSerializeData.h"
#include "../ErrorManager/ErrorManager.h"
#include "../Math/MathGeneral.h"
#include "../Memory/SigmaMemory.h"
#include "../TypeDescriptor/TypeDescriptor.h"
#include "../TypeDescriptor/TypeDescriptorField.h"

// Defined in X/Sigma/Files/XFilesCore.cpp. XFiles.h does not declare them yet, so they are
// declared here (the same way X/Client/Definitions/PreferencesDefinition.cpp does).
BOOL FileOpen(FileReference & tFileRef, uint32 dwAccessFlags);
void FileClose(FileReference & tFileRef);
BOOL FileRead(FileReference & tFileRef, void * lpBuf, const uint32 dwOffset, const uint32 dwSize, const ErrorCode eErrorCodeIfFails);
BOOL FileGetSize(const FileReference & tFileRef, uint32 & dwSize);
LPCSTR FileReferenceGetPath(const FileReference & tFileRef);
void FileSplitPath(LPCSTR szPath, LPSTR pszDrive, LPSTR pszDirectory, LPSTR pszFilename, LPSTR pszExtension);

// The original also defines `static const uint32 MAX_CACHED_FILE_ENTRIES` (line 22) and the
// static inline helpers sFileReadSequential (line 378) and sCacheListIsUpToDate (line 446),
// which no emitted function uses; they are left out, so nothing is emitted for them either.

static const TypeDescriptorField SerializeData_Fields[3] = {  // line 17
    TypeDescriptorField("", DT_INT, 0x00, NULL, 1),
    TypeDescriptorField("", DT_INT, 0x04, NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, 0x08, NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SerializeData("SerializeData", 0, SerializeData_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 20
const XTypeDescriptor * DT_SERIALIZEDATA = &TypeDescriptor_SerializeData;

void * FileReadIntoMemory(FileReference & tFileRef, uint32 & dwSize, const ErrorCode eErrorCodeIfFails)  // line 33
{
    if (!FileOpen(tFileRef, 1 << ACCESS_READ_BIT))
        return NULL;

    FileGetSize(tFileRef, dwSize);
    void * lpBuffer = SigmaMemoryNew(dwSize, 4, NULL, FALSE);
    if (!FileRead(tFileRef, lpBuffer, 0, dwSize, eErrorCodeIfFails))
    {
        SigmaMemoryFree(lpBuffer, NULL);
        lpBuffer = NULL;
    }
    FileClose(tFileRef);
    return lpBuffer;
}

void FileSerialize(SerializeData & ser, uint32 & uCurrentOffset, uint32 uByteAlignment)  // line 172
{
    // The test against one is FileSerialize's own (original line 180, cmp/csel), not Math::Align's (inlined at 182).
    ser.dwOffset = (uByteAlignment == 1) ? uCurrentOffset : (uint32)Math::Align(uCurrentOffset, uByteAlignment);
    uCurrentOffset = ser.dwSize + ser.dwOffset;
}

void FileReferenceGetNameNoExtension(FileReference & tFileRef, LPSTR szFileNameNoExtension)  // line 209
{
    LPCSTR szPath = FileReferenceGetPath(tFileRef);
    FileSplitPath(szPath, NULL, NULL, szFileNameNoExtension, NULL);
}

void FileReferenceGetExtension(FileReference & tFileRef, LPSTR szExtension)  // line 221
{
    LPCSTR szPath = FileReferenceGetPath(tFileRef);
    FileSplitPath(szPath, NULL, NULL, NULL, szExtension);
}
