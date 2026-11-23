/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2016, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/


/* Original DWARF declarations consumed by bundles.c, CU DIE 0x07c8ece6.
 * Complete definitions for every by-value type; pointer-only types are forward
 * declarations. No synthetic padding, hidden implementation body or API stub.
 */
#ifndef D3_RECOVERED_EXTERNALS_CURL_LIB_FTP_H
#define D3_RECOVERED_EXTERNALS_CURL_LIB_FTP_H

#include <stddef.h>
#include <stdint.h>
#include "Externals/curl/lib/pingpong.h"
#include "Programs/Eris/Outputs/Include/curl/curlbuild.h"

struct pingpong;

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/ftp.h:80; DIE 0x7c92302 */
typedef enum {
  FTP_STOP = 0,
  FTP_WAIT220 = 1,
  FTP_AUTH = 2,
  FTP_USER = 3,
  FTP_PASS = 4,
  FTP_ACCT = 5,
  FTP_PBSZ = 6,
  FTP_PROT = 7,
  FTP_CCC = 8,
  FTP_PWD = 9,
  FTP_SYST = 10,
  FTP_NAMEFMT = 11,
  FTP_QUOTE = 12,
  FTP_RETR_PREQUOTE = 13,
  FTP_STOR_PREQUOTE = 14,
  FTP_POSTQUOTE = 15,
  FTP_CWD = 16,
  FTP_MKD = 17,
  FTP_MDTM = 18,
  FTP_TYPE = 19,
  FTP_LIST_TYPE = 20,
  FTP_RETR_TYPE = 21,
  FTP_STOR_TYPE = 22,
  FTP_SIZE = 23,
  FTP_RETR_SIZE = 24,
  FTP_STOR_SIZE = 25,
  FTP_REST = 26,
  FTP_RETR_REST = 27,
  FTP_PORT = 28,
  FTP_PRET = 29,
  FTP_PASV = 30,
  FTP_LIST = 31,
  FTP_RETR = 32,
  FTP_STOR = 33,
  FTP_QUIT = 34,
  FTP_LAST = 35
} ftpstate;

_Static_assert(sizeof(ftpstate) == 4, "ftpstate size");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/ftp.h:118; DIE 0x7c9211c */
struct ftp_conn {
  struct pingpong pp; /* 0x0 */
  char *entrypath; /* 0x70 */
  char **dirs; /* 0x78 */
  int dirdepth; /* 0x80 */
  int diralloc; /* 0x84 */
  char *file; /* 0x88 */
  _Bool dont_check; /* 0x90 */
  _Bool ctl_valid; /* 0x91 */
  _Bool cwddone; /* 0x92 */
  _Bool cwdfail; /* 0x93 */
  _Bool wait_data_conn; /* 0x94 */
  char *prevpath; /* 0x98 */
  char transfertype; /* 0xa0 */
  int count1; /* 0xa4 */
  int count2; /* 0xa8 */
  int count3; /* 0xac */
  ftpstate state; /* 0xb0 */
  ftpstate state_saved; /* 0xb4 */
  curl_off_t retr_size_saved; /* 0xb8 */
  char *server_os; /* 0xc0 */
  curl_off_t known_filesize; /* 0xc8 */
  char *newhost; /* 0xd0 */
  unsigned short newport; /* 0xd8 */
};

_Static_assert(sizeof(struct ftp_conn) == 224, "ftp_conn size");
_Static_assert(offsetof(struct ftp_conn, pp) == 0, "ftp_conn.pp");
_Static_assert(offsetof(struct ftp_conn, entrypath) == 112, "ftp_conn.entrypath");
_Static_assert(offsetof(struct ftp_conn, dirs) == 120, "ftp_conn.dirs");
_Static_assert(offsetof(struct ftp_conn, dirdepth) == 128, "ftp_conn.dirdepth");
_Static_assert(offsetof(struct ftp_conn, diralloc) == 132, "ftp_conn.diralloc");
_Static_assert(offsetof(struct ftp_conn, file) == 136, "ftp_conn.file");
_Static_assert(offsetof(struct ftp_conn, dont_check) == 144, "ftp_conn.dont_check");
_Static_assert(offsetof(struct ftp_conn, ctl_valid) == 145, "ftp_conn.ctl_valid");
_Static_assert(offsetof(struct ftp_conn, cwddone) == 146, "ftp_conn.cwddone");
_Static_assert(offsetof(struct ftp_conn, cwdfail) == 147, "ftp_conn.cwdfail");
_Static_assert(offsetof(struct ftp_conn, wait_data_conn) == 148, "ftp_conn.wait_data_conn");
_Static_assert(offsetof(struct ftp_conn, prevpath) == 152, "ftp_conn.prevpath");
_Static_assert(offsetof(struct ftp_conn, transfertype) == 160, "ftp_conn.transfertype");
_Static_assert(offsetof(struct ftp_conn, count1) == 164, "ftp_conn.count1");
_Static_assert(offsetof(struct ftp_conn, count2) == 168, "ftp_conn.count2");
_Static_assert(offsetof(struct ftp_conn, count3) == 172, "ftp_conn.count3");
_Static_assert(offsetof(struct ftp_conn, state) == 176, "ftp_conn.state");
_Static_assert(offsetof(struct ftp_conn, state_saved) == 180, "ftp_conn.state_saved");
_Static_assert(offsetof(struct ftp_conn, retr_size_saved) == 184, "ftp_conn.retr_size_saved");
_Static_assert(offsetof(struct ftp_conn, server_os) == 192, "ftp_conn.server_os");
_Static_assert(offsetof(struct ftp_conn, known_filesize) == 200, "ftp_conn.known_filesize");
_Static_assert(offsetof(struct ftp_conn, newhost) == 208, "ftp_conn.newhost");
_Static_assert(offsetof(struct ftp_conn, newport) == 216, "ftp_conn.newport");

/* Added for conncache.c (struct UserDefined). */
/* D:/home/teamcity2/work/sdk/Externals/curl/lib/ftp.h:98 */
typedef enum {
  FTPFILE_MULTICWD = 1,
  FTPFILE_NOCWD = 2,
  FTPFILE_SINGLECWD = 3
} curl_ftpfile;

#endif /* D3_RECOVERED_EXTERNALS_CURL_LIB_FTP_H */
