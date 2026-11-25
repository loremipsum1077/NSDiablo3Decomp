#include "buildver.h"

int D3VersionMajor() { return 2; }
int D3VersionMinor() { return 7; }
int D3VersionRevision() { return 7; }
int D3BuildNum() { return 92380; }
int D3CodeVersion() { return 135863; }
int D3DataVersion() { return 1751824; }
const char* D3CodeBranch() { return "branches/2_7_7"; }
const char* D3DataBranch() { return "branches/2_7_7"; }
const char* BuildVersion() { return "2.7.7"; }
const char* BuildVersionFull() { return "C2.7.7.92380"; }
const char* BuildVersionCodeData() { return "C2.7.7.92380 (135863-1751824)"; }

const char* BuildConfigStr() {
    return "";
}
