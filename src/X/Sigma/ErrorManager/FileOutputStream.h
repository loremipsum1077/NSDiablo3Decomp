#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "ErrorManagerLogging.h"
#include "../SigmaTypes.h"
#include "../Unicode.h"

class FileOutputStream {  // size 0x10c0
public:
    FileOutputStream();
    ~FileOutputStream();
    BOOL IsInitialized() { return m_bInitialized; }  // line 25
    void SetSeverityLevel(SeverityLevel eLevel) { m_eSeverityLevel = eLevel; }  // line 26
    SeverityLevel GetSeverityLevel() { return m_eSeverityLevel; }  // line 27
    void SetRingBufferSize(uint32);
    const RingBufferLogInfo & GetRingBufferLog() const;
    void DumpRingBuffer();
    void SetFileName(LPCSTR pszFilePath);
    const char * GetFileName() { return m_szFilename; }  // line 42
    void Log(BOOL bAddTimestamp, LPCSTR szMessage, LPCSTR szPrependedLabel);
    void LogToRingBuffer(BOOL bAddTimestamp, LPCSTR szMessage, LPCSTR szPrependedLabel);
    const char * GetFilePath() { return m_szFilePath; }  // line 57
    void Initialize(LPCSTR szLogPath);
private:
    FileOutputStream(const FileOutputStream &);
    const FileOutputStream & operator=(const FileOutputStream &);
    RingBufferLogInfo m_tRingBufferLog;  // 0x00
    BOOL m_bInitialized;  // 0x10
    Blizzard::Lock::RMutex m_criticalSection;  // 0x18
    CHAR m_szFilename[128];  // 0x38
    CHAR m_szFilePath[4096];  // 0xb8
    SeverityLevel m_eSeverityLevel;  // 0x10b8
};
static_assert(sizeof(FileOutputStream) == 0x10c0, "FileOutputStream");
