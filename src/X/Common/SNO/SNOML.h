// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/SNO/SNOML.h

#pragma once

enum SNOMLResult {
    SNOML_NOFILECREATED = 0,
    SNOML_FILEEXISTS = 1,
    SNOML_FILECREATED = 2,
    SNOML_READONLY = 3,
    SNOML_FAILED = 4,
};

// SNOML_Shared.cpp:109 and :130 (X/Common/Game.cpp sFileSystemInitialize / sFileSystemDispose).
class GroupDefinition;
void SNOMLInitialize(GroupDefinition ** ptGameDefinitions);
void SNOMLDispose();
