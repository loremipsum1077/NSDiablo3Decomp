// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: BlizzardCore/Include/Blizzard/Mopaq.h

#pragma once

#include "Types.h"

namespace Blizzard {
typedef int64 IOOffset;

}  // namespace Blizzard
typedef uint64 HSARCHIVE;

namespace Blizzard {
namespace Mopaq {
typedef uint32 OpenArchiveFlags;

enum FileExistsResult {
    SFILEEXIST_FAIL = 0,
    SFILEEXIST_MPQ = 1,
    SFILEEXIST_MPQONDISK = 2,
    SFILEEXIST_DISK = 3,
    SFILEEXIST_DELETED = 4,
    SFILEEXIST_BLIZPAQ = 5,
};

enum OpenFileResult {
    SFILEOPEN_FAIL = 0,
    SFILEOPEN_MPQ = 1,
    SFILEOPEN_DISK = 2,
};

typedef uint32 OpenFileFlags;

enum ErrorCode {
    SFILE_NOERR = 0,
    SFILE_ERROR_FILE_NOT_FOUND = 2,
    SFILE_ERROR_ACCESS_DENIED = 5,
    SFILE_ERROR_INVALID_HANDLE = 6,
    SFILE_ERROR_INVALID_DRIVE = 15,
    SFILE_ERROR_HANDLE_EOF = 38,
    SFILE_ERROR_INVALID_PARAMETER = 87,
    SFILE_ERROR_NO_LISTFILE = 103,
    SFILE_ERROR_FILE_IO_ERROR = 104,
    SFILE_ERROR_HASH_TABLE_FULL = 105,
    SFILE_ERROR_DUPLICATE_FILE = 106,
    SFILE_ERROR_BLOCK_TABLE_FULL = 107,
    SFILE_ERROR_NOT_ARCHIVE = 108,
    SFILE_ERROR_SIGNING_FAILURE = 109,
    SFILE_ERROR_INVALID_PARAMETERS = 110,
    SFILE_ERROR_NOT_IN_ARCHIVE = 111,
    SFILE_ERROR_DISK_FULL = 112,
    SFILE_ERROR_FILE_CORRUPT = 131,
    SFILE_ERROR_BAD_PATHNAME = 161,
    SFILE_ERROR_FILE_INVALID = 1006,
    SFILE_ERROR_NOT_AUTHENTICATED = 1244,
    SFILE_ERROR_BAD_COPY_RANGE = 2000,
    SFILE_ERROR_CANNOT_CREATE_COPY_FILE = 2001,
    SFILE_ERROR_COPY_READ_FAILED = 2002,
    SFILE_ERROR_COPY_WRITE_FAILED = 2003,
    SFILE_ERROR_COPY_CLOSE_FAILED = 2004,
    SFILE_ERROR_OPEN_FAILED = 2012,
    SFILE_ERROR_DOWNLOAD_FAILED = 2100,
    SFILE_ERROR_ARCHIVE_TABLE_INVALID = 2200,
    SFILE_ERROR_ARCHIVE_OUT_OF_HANDLES = 2201,
};

}  // namespace Mopaq
}  // namespace Blizzard

// Variables defined in this file:
//   line    49  static const HSARCHIVE INVALID_ARCHIVE_HANDLE
//   line   112  static const Blizzard::Mopaq::OpenArchiveFlags Blizzard::Mopaq::SFILE_ARCHIVE_DONT_SEARCH
//   line   119  static const Blizzard::Mopaq::OpenArchiveFlags Blizzard::Mopaq::SFILE_ARCHIVE_USE_NEW_BLOCK_HASH_FORMAT
//   line   120  static const Blizzard::Mopaq::OpenArchiveFlags Blizzard::Mopaq::SFILE_ARCHIVE_INIT_TABLE_ON_OPEN
//   line   121  static const Blizzard::Mopaq::OpenArchiveFlags Blizzard::Mopaq::SFILE_ARCHIVE_LOAD_DELTA_VALUES
//   line   164  static const Blizzard::Mopaq::OpenFileFlags Blizzard::Mopaq::SFILE_OPENFLAG_CHECKDISK
//   line   168  static const Blizzard::Mopaq::OpenFileFlags Blizzard::Mopaq::SFILE_OPENFLAG_PERM_SHARED_WRITE
