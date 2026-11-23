// Minimal declaration: only the nested visibility enum (DWARF bdFileInfo.h:25) that
// other bdStorage headers need. The rest of bdFileInfo is not adopted yet.
#pragma once

class bdFileInfo {
public:
    enum bdVisibility {
        BD_VISIBLE_PUBLIC = 0,
        BD_VISIBLE_PRIVATE = 1,
        BD_MAX_VISIBILITY_TYPE = 2
    };
};
