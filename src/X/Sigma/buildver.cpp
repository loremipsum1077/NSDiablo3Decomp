#include "buildver.h"

int D3VersionMajor() { return 2; }
int D3VersionMinor() { return 6; }
int D3VersionRevision() { return 2; }
int D3BuildNum() { return 52309; }
int D3CodeVersion() { return 133284; }
int D3DataVersion() { return 1615508; }
const char* D3CodeBranch() { return "branches/2_6_2"; }
const char* D3DataBranch() { return "branches/2_6_2"; }
const char* BuildVersion() { return "2.6.2"; }
const char* BuildVersionFull() { return "C2.6.2.52309"; }
const char* BuildVersionCodeData() { return "C2.6.2.52309 (133284-1615508)"; }

const char* BuildConfigStr() {
    return "";
}
