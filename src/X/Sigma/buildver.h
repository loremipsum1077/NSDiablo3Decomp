#pragma once

struct BuildVersions {
    BuildVersions();
    int versionMajor;
    int versionMinor;
    int versionRevision;
    int versionBuildNumber;
};

int D3VersionMajor();
int D3VersionMinor();
int D3VersionRevision();
int D3BuildNum();
int D3CodeVersion();
int D3DataVersion();
const char* D3CodeBranch();
const char* D3DataBranch();
const char* BuildVersion();
const char* BuildVersionFull();
const char* BuildVersionCodeData();
const char* BuildConfigStr();

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(BuildVersions) == 0x10, "BuildVersions");
static_assert(offsetof(BuildVersions, versionMajor) == 0x0, "BuildVersions");
static_assert(offsetof(BuildVersions, versionMinor) == 0x4, "BuildVersions");
static_assert(offsetof(BuildVersions, versionRevision) == 0x8, "BuildVersions");
static_assert(offsetof(BuildVersions, versionBuildNumber) == 0xc, "BuildVersions");
