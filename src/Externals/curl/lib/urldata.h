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
#ifndef D3_RECOVERED_EXTERNALS_CURL_LIB_URLDATA_H
#define D3_RECOVERED_EXTERNALS_CURL_LIB_URLDATA_H

#include <stddef.h>
#include <stdint.h>
#include "Externals/curl/lib/curl_addrinfo.h"
#include "Externals/curl/lib/ftp.h"
#include "Externals/curl/lib/http.h"
#include "Externals/curl/lib/http_chunks.h"
#include "Externals/curl/lib/imap.h"
#include "Externals/curl/lib/pop3.h"
#include "Externals/curl/lib/rtsp.h"
#include "Externals/curl/lib/smb.h"
#include "Externals/curl/lib/smtp.h"
#include "Externals/curl/lib/ssh.h"
#include "NintendoSDK/Compilers/NX/nx/aarch64/include/bits/alltypes.h"
#include "Programs/Eris/Include/nnc/ssl/ssl_Connection.h"
#include "Programs/Eris/Include/nnc/ssl/ssl_Context.h"
#include "Programs/Eris/Outputs/Include/curl/curl.h"
#include <stdio.h>
#include "Externals/curl/lib/conncache.h"
#include "Externals/curl/lib/multihandle.h"
#include "Externals/curl/lib/splay.h"
#include "Externals/curl/lib/wildcard.h"

struct ConnectBits;
struct Curl_async;
struct Curl_chunker;
struct Curl_dns_entry;
struct Curl_handler;
struct SessionHandle;
struct connectbundle;
struct connectdata;
struct curl_llist;
struct dynamically_allocated_data;
struct ftp_conn;
struct hostname;
struct http_conn;
struct imap_conn;
struct pop3_conn;
struct rtsp_conn;
struct smb_conn;
struct smtp_conn;
struct ssh_conn;
struct ssl_config_data;
struct ssl_connect_data;
struct tftp_state_data;
struct timeval;

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:255; DIE 0x7c91cfb */
typedef enum {
  ssl_connect_1 = 0,
  ssl_connect_2 = 1,
  ssl_connect_2_reading = 2,
  ssl_connect_2_writing = 3,
  ssl_connect_3 = 4,
  ssl_connect_done = 5
} ssl_connect_state;

_Static_assert(sizeof(ssl_connect_state) == 4, "ssl_connect_state size");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:261; DIE 0x7c91c86 */
typedef enum {
  ssl_connection_none = 0,
  ssl_connection_negotiating = 1,
  ssl_connection_complete = 2
} ssl_connection_state;

_Static_assert(sizeof(ssl_connection_state) == 4, "ssl_connection_state size");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:346; DIE 0x7c90ceb */
struct ssl_config_data {
  long version; /* 0x0 */
  long certverifyresult; /* 0x8 */
  _Bool verifypeer; /* 0x10 */
  _Bool verifyhost; /* 0x11 */
  _Bool verifystatus; /* 0x12 */
  _Bool verifydate; /* 0x13 */
  void *psslctx; /* 0x18 */
  long dohandshakeresult; /* 0x20 */
  curl_ssl_conn_callback_before fsslconnbefore; /* 0x28 */
  curl_ssl_conn_callback_after fsslconnafter; /* 0x30 */
  void *psslcondata; /* 0x38 */
  _Bool skipssldefaultverify; /* 0x40 */
  char *CApath; /* 0x48 */
  char *CAfile; /* 0x50 */
  const char *CRLfile; /* 0x58 */
  const char *issuercert; /* 0x60 */
  char *random_file; /* 0x68 */
  char *egdsocket; /* 0x70 */
  char *cipher_list; /* 0x78 */
  size_t max_ssl_sessions; /* 0x80 */
  curl_ssl_ctx_callback fsslctx; /* 0x88 */
  void *fsslctxp; /* 0x90 */
  _Bool sessionid; /* 0x98 */
  _Bool certinfo; /* 0x99 */
  _Bool falsestart; /* 0x9a */
};

_Static_assert(sizeof(struct ssl_config_data) == 160, "ssl_config_data size");
_Static_assert(offsetof(struct ssl_config_data, version) == 0, "ssl_config_data.version");
_Static_assert(offsetof(struct ssl_config_data, certverifyresult) == 8, "ssl_config_data.certverifyresult");
_Static_assert(offsetof(struct ssl_config_data, verifypeer) == 16, "ssl_config_data.verifypeer");
_Static_assert(offsetof(struct ssl_config_data, verifyhost) == 17, "ssl_config_data.verifyhost");
_Static_assert(offsetof(struct ssl_config_data, verifystatus) == 18, "ssl_config_data.verifystatus");
_Static_assert(offsetof(struct ssl_config_data, verifydate) == 19, "ssl_config_data.verifydate");
_Static_assert(offsetof(struct ssl_config_data, psslctx) == 24, "ssl_config_data.psslctx");
_Static_assert(offsetof(struct ssl_config_data, dohandshakeresult) == 32, "ssl_config_data.dohandshakeresult");
_Static_assert(offsetof(struct ssl_config_data, fsslconnbefore) == 40, "ssl_config_data.fsslconnbefore");
_Static_assert(offsetof(struct ssl_config_data, fsslconnafter) == 48, "ssl_config_data.fsslconnafter");
_Static_assert(offsetof(struct ssl_config_data, psslcondata) == 56, "ssl_config_data.psslcondata");
_Static_assert(offsetof(struct ssl_config_data, skipssldefaultverify) == 64, "ssl_config_data.skipssldefaultverify");
_Static_assert(offsetof(struct ssl_config_data, CApath) == 72, "ssl_config_data.CApath");
_Static_assert(offsetof(struct ssl_config_data, CAfile) == 80, "ssl_config_data.CAfile");
_Static_assert(offsetof(struct ssl_config_data, CRLfile) == 88, "ssl_config_data.CRLfile");
_Static_assert(offsetof(struct ssl_config_data, issuercert) == 96, "ssl_config_data.issuercert");
_Static_assert(offsetof(struct ssl_config_data, random_file) == 104, "ssl_config_data.random_file");
_Static_assert(offsetof(struct ssl_config_data, egdsocket) == 112, "ssl_config_data.egdsocket");
_Static_assert(offsetof(struct ssl_config_data, cipher_list) == 120, "ssl_config_data.cipher_list");
_Static_assert(offsetof(struct ssl_config_data, max_ssl_sessions) == 128, "ssl_config_data.max_ssl_sessions");
_Static_assert(offsetof(struct ssl_config_data, fsslctx) == 136, "ssl_config_data.fsslctx");
_Static_assert(offsetof(struct ssl_config_data, fsslctxp) == 144, "ssl_config_data.fsslctxp");
_Static_assert(offsetof(struct ssl_config_data, sessionid) == 152, "ssl_config_data.sessionid");
_Static_assert(offsetof(struct ssl_config_data, certinfo) == 153, "ssl_config_data.certinfo");
_Static_assert(offsetof(struct ssl_config_data, falsestart) == 154, "ssl_config_data.falsestart");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:499; DIE 0x7c91d06 */
struct ConnectBits {
  _Bool close; /* 0x0 */
  _Bool reuse; /* 0x1 */
  _Bool proxy; /* 0x2 */
  _Bool httpproxy; /* 0x3 */
  _Bool user_passwd; /* 0x4 */
  _Bool proxy_user_passwd; /* 0x5 */
  _Bool ipv6_ip; /* 0x6 */
  _Bool ipv6; /* 0x7 */
  _Bool do_more; /* 0x8 */
  _Bool tcpconnect[2]; /* 0x9 */
  _Bool protoconnstart; /* 0xb */
  _Bool retry; /* 0xc */
  _Bool tunnel_proxy; /* 0xd */
  _Bool authneg; /* 0xe */
  _Bool rewindaftersend; /* 0xf */
  _Bool ftp_use_epsv; /* 0x10 */
  _Bool ftp_use_eprt; /* 0x11 */
  _Bool netrc; /* 0x12 */
  _Bool userpwd_in_url; /* 0x13 */
  _Bool done; /* 0x14 */
  _Bool stream_was_rewound; /* 0x15 */
  _Bool proxy_connect_closed; /* 0x16 */
  _Bool bound; /* 0x17 */
  _Bool type_set; /* 0x18 */
};

_Static_assert(sizeof(struct ConnectBits) == 25, "ConnectBits size");
_Static_assert(offsetof(struct ConnectBits, close) == 0, "ConnectBits.close");
_Static_assert(offsetof(struct ConnectBits, reuse) == 1, "ConnectBits.reuse");
_Static_assert(offsetof(struct ConnectBits, proxy) == 2, "ConnectBits.proxy");
_Static_assert(offsetof(struct ConnectBits, httpproxy) == 3, "ConnectBits.httpproxy");
_Static_assert(offsetof(struct ConnectBits, user_passwd) == 4, "ConnectBits.user_passwd");
_Static_assert(offsetof(struct ConnectBits, proxy_user_passwd) == 5, "ConnectBits.proxy_user_passwd");
_Static_assert(offsetof(struct ConnectBits, ipv6_ip) == 6, "ConnectBits.ipv6_ip");
_Static_assert(offsetof(struct ConnectBits, ipv6) == 7, "ConnectBits.ipv6");
_Static_assert(offsetof(struct ConnectBits, do_more) == 8, "ConnectBits.do_more");
_Static_assert(offsetof(struct ConnectBits, tcpconnect) == 9, "ConnectBits.tcpconnect");
_Static_assert(offsetof(struct ConnectBits, protoconnstart) == 11, "ConnectBits.protoconnstart");
_Static_assert(offsetof(struct ConnectBits, retry) == 12, "ConnectBits.retry");
_Static_assert(offsetof(struct ConnectBits, tunnel_proxy) == 13, "ConnectBits.tunnel_proxy");
_Static_assert(offsetof(struct ConnectBits, authneg) == 14, "ConnectBits.authneg");
_Static_assert(offsetof(struct ConnectBits, rewindaftersend) == 15, "ConnectBits.rewindaftersend");
_Static_assert(offsetof(struct ConnectBits, ftp_use_epsv) == 16, "ConnectBits.ftp_use_epsv");
_Static_assert(offsetof(struct ConnectBits, ftp_use_eprt) == 17, "ConnectBits.ftp_use_eprt");
_Static_assert(offsetof(struct ConnectBits, netrc) == 18, "ConnectBits.netrc");
_Static_assert(offsetof(struct ConnectBits, userpwd_in_url) == 19, "ConnectBits.userpwd_in_url");
_Static_assert(offsetof(struct ConnectBits, done) == 20, "ConnectBits.done");
_Static_assert(offsetof(struct ConnectBits, stream_was_rewound) == 21, "ConnectBits.stream_was_rewound");
_Static_assert(offsetof(struct ConnectBits, proxy_connect_closed) == 22, "ConnectBits.proxy_connect_closed");
_Static_assert(offsetof(struct ConnectBits, bound) == 23, "ConnectBits.bound");
_Static_assert(offsetof(struct ConnectBits, type_set) == 24, "ConnectBits.type_set");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:557; DIE 0x7c91b3e */
struct hostname {
  char *rawalloc; /* 0x0 */
  char *encalloc; /* 0x8 */
  char *name; /* 0x10 */
  const char *dispname; /* 0x18 */
};

_Static_assert(sizeof(struct hostname) == 32, "hostname size");
_Static_assert(offsetof(struct hostname, rawalloc) == 0, "hostname.rawalloc");
_Static_assert(offsetof(struct hostname, encalloc) == 8, "hostname.encalloc");
_Static_assert(offsetof(struct hostname, name) == 16, "hostname.name");
_Static_assert(offsetof(struct hostname, dispname) == 24, "hostname.dispname");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:593; DIE 0x7c920c4 */
struct Curl_async {
  char *hostname; /* 0x0 */
  int port; /* 0x8 */
  struct Curl_dns_entry *dns; /* 0x10 */
  _Bool done; /* 0x18 */
  int status; /* 0x1c */
  void *os_specific; /* 0x20 */
};

_Static_assert(sizeof(struct Curl_async) == 40, "Curl_async size");
_Static_assert(offsetof(struct Curl_async, hostname) == 0, "Curl_async.hostname");
_Static_assert(offsetof(struct Curl_async, port) == 8, "Curl_async.port");
_Static_assert(offsetof(struct Curl_async, dns) == 16, "Curl_async.dns");
_Static_assert(offsetof(struct Curl_async, done) == 24, "Curl_async.done");
_Static_assert(offsetof(struct Curl_async, status) == 28, "Curl_async.status");
_Static_assert(offsetof(struct Curl_async, os_specific) == 32, "Curl_async.os_specific");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:830; DIE 0x7c91be6 */
typedef ssize_t Curl_send(struct connectdata *, int, const void *, size_t, CURLcode *);


/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:837; DIE 0x7c91ba5 */
typedef ssize_t Curl_recv(struct connectdata *, int, char *, size_t, CURLcode *);


/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:965; DIE 0x7c9202b */
struct dynamically_allocated_data {
  char *proxyuserpwd; /* 0x0 */
  char *uagent; /* 0x8 */
  char *accept_encoding; /* 0x10 */
  char *userpwd; /* 0x18 */
  char *rangeline; /* 0x20 */
  char *ref; /* 0x28 */
  char *host; /* 0x30 */
  char *cookiehost; /* 0x38 */
  char *rtsp_transport; /* 0x40 */
  char *te; /* 0x48 */
};

_Static_assert(sizeof(struct dynamically_allocated_data) == 80, "dynamically_allocated_data size");
_Static_assert(offsetof(struct dynamically_allocated_data, proxyuserpwd) == 0, "dynamically_allocated_data.proxyuserpwd");
_Static_assert(offsetof(struct dynamically_allocated_data, uagent) == 8, "dynamically_allocated_data.uagent");
_Static_assert(offsetof(struct dynamically_allocated_data, accept_encoding) == 16, "dynamically_allocated_data.accept_encoding");
_Static_assert(offsetof(struct dynamically_allocated_data, userpwd) == 24, "dynamically_allocated_data.userpwd");
_Static_assert(offsetof(struct dynamically_allocated_data, rangeline) == 32, "dynamically_allocated_data.rangeline");
_Static_assert(offsetof(struct dynamically_allocated_data, ref) == 40, "dynamically_allocated_data.ref");
_Static_assert(offsetof(struct dynamically_allocated_data, host) == 48, "dynamically_allocated_data.host");
_Static_assert(offsetof(struct dynamically_allocated_data, cookiehost) == 56, "dynamically_allocated_data.cookiehost");
_Static_assert(offsetof(struct dynamically_allocated_data, rtsp_transport) == 64, "dynamically_allocated_data.rtsp_transport");
_Static_assert(offsetof(struct dynamically_allocated_data, te) == 72, "dynamically_allocated_data.te");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:264; DIE 0x7c91c1d */
struct ssl_connect_data {
  _Bool use; /* 0x0 */
  ssl_connection_state state; /* 0x4 */
  nnsslSslConnectionType nnssl_connection; /* 0x8 */
  nnsslSslContextType nnssl_context; /* 0x88 */
  nnsslSslContextType *pnnssl_context; /* 0x108 */
  _Bool using_external_ssl_context; /* 0x110 */
  ssl_connect_state connecting_state; /* 0x114 */
};

_Static_assert(sizeof(struct ssl_connect_data) == 280, "ssl_connect_data size");
_Static_assert(offsetof(struct ssl_connect_data, use) == 0, "ssl_connect_data.use");
_Static_assert(offsetof(struct ssl_connect_data, state) == 4, "ssl_connect_data.state");
_Static_assert(offsetof(struct ssl_connect_data, nnssl_connection) == 8, "ssl_connect_data.nnssl_connection");
_Static_assert(offsetof(struct ssl_connect_data, nnssl_context) == 136, "ssl_connect_data.nnssl_context");
_Static_assert(offsetof(struct ssl_connect_data, pnnssl_context) == 264, "ssl_connect_data.pnnssl_context");
_Static_assert(offsetof(struct ssl_connect_data, using_external_ssl_context) == 272, "ssl_connect_data.using_external_ssl_context");
_Static_assert(offsetof(struct ssl_connect_data, connecting_state) == 276, "ssl_connect_data.connecting_state");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:847; DIE 0x7c8f28d */
struct connectdata {
  struct SessionHandle *data; /* 0x0 */
  struct Curl_chunker chunk; /* 0x8 */
  curl_closesocket_callback fclosesocket; /* 0x38 */
  void *closesocket_client; /* 0x40 */
  _Bool inuse; /* 0x48 */
  long connection_id; /* 0x50 */
  struct Curl_dns_entry *dns_entry; /* 0x58 */
  Curl_addrinfo *ip_addr; /* 0x60 */
  Curl_addrinfo *tempaddr[2]; /* 0x68 */
  char ip_addr_str[46]; /* 0x78 */
  unsigned int scope_id; /* 0xa8 */
  int socktype; /* 0xac */
  struct hostname host; /* 0xb0 */
  struct hostname proxy; /* 0xd0 */
  long port; /* 0xf0 */
  int remote_port; /* 0xf8 */
  char primary_ip[46]; /* 0xfc */
  long primary_port; /* 0x130 */
  char local_ip[46]; /* 0x138 */
  long local_port; /* 0x168 */
  char *user; /* 0x170 */
  char *passwd; /* 0x178 */
  char *options; /* 0x180 */
  char *xoauth2_bearer; /* 0x188 */
  char *proxyuser; /* 0x190 */
  char *proxypasswd; /* 0x198 */
  curl_proxytype proxytype; /* 0x1a0 */
  int httpversion; /* 0x1a4 */
  int rtspversion; /* 0x1a8 */
  struct timeval now; /* 0x1b0 */
  struct timeval created; /* 0x1c0 */
  curl_socket_t sock[2]; /* 0x1d0 */
  curl_socket_t tempsock[2]; /* 0x1d8 */
  _Bool sock_accepted[2]; /* 0x1e0 */
  Curl_recv *recv[2]; /* 0x1e8 */
  Curl_send *send[2]; /* 0x1f8 */
  struct ssl_connect_data ssl[2]; /* 0x208 */
  struct ssl_config_data ssl_config; /* 0x438 */
  struct ConnectBits bits; /* 0x4d8 */
  struct timeval connecttime; /* 0x4f8 */
  int num_addr; /* 0x508 */
  long timeoutms_per_addr; /* 0x510 */
  const struct Curl_handler *handler; /* 0x518 */
  const struct Curl_handler *given; /* 0x520 */
  long ip_version; /* 0x528 */
  curl_socket_t sockfd; /* 0x530 */
  curl_socket_t writesockfd; /* 0x534 */
  struct dynamically_allocated_data allocptr; /* 0x538 */
  _Bool readchannel_inuse; /* 0x588 */
  _Bool writechannel_inuse; /* 0x589 */
  struct curl_llist *send_pipe; /* 0x590 */
  struct curl_llist *recv_pipe; /* 0x598 */
  char *master_buffer; /* 0x5a0 */
  size_t read_pos; /* 0x5a8 */
  size_t buf_len; /* 0x5b0 */
  curl_seek_callback seek_func; /* 0x5b8 */
  void *seek_client; /* 0x5c0 */
  curl_read_callback fread_func; /* 0x5c8 */
  void *fread_in; /* 0x5d0 */
  char syserr_buf[256]; /* 0x5d8 */
  struct Curl_async async; /* 0x6d8 */
  char *trailer; /* 0x700 */
  int trlMax; /* 0x708 */
  int trlPos; /* 0x70c */
  union {
    struct ftp_conn ftpc; /* 0x0 */
    struct http_conn httpc; /* 0x0 */
    struct ssh_conn sshc; /* 0x0 */
    struct tftp_state_data *tftpc; /* 0x0 */
    struct imap_conn imapc; /* 0x0 */
    struct pop3_conn pop3c; /* 0x0 */
    struct smtp_conn smtpc; /* 0x0 */
    struct rtsp_conn rtspc; /* 0x0 */
    struct smb_conn smbc; /* 0x0 */
    void *generic; /* 0x0 */
  } proto; /* 0x710 */
  int cselect_bits; /* 0x7f0 */
  int waitfor; /* 0x7f4 */
  _Bool verifypeer; /* 0x7f8 */
  _Bool verifyhost; /* 0x7f9 */
  char *localdev; /* 0x800 */
  unsigned short localport; /* 0x808 */
  int localportrange; /* 0x80c */
  enum {
    TUNNEL_INIT = 0,
    TUNNEL_CONNECT = 1,
    TUNNEL_COMPLETE = 2
  } tunnel_state[2]; /* 0x810 */
  struct connectbundle *bundle; /* 0x818 */
  int negnpn; /* 0x820 */
};

_Static_assert(sizeof(struct connectdata) == 2088, "connectdata size");
_Static_assert(offsetof(struct connectdata, data) == 0, "connectdata.data");
_Static_assert(offsetof(struct connectdata, chunk) == 8, "connectdata.chunk");
_Static_assert(offsetof(struct connectdata, fclosesocket) == 56, "connectdata.fclosesocket");
_Static_assert(offsetof(struct connectdata, closesocket_client) == 64, "connectdata.closesocket_client");
_Static_assert(offsetof(struct connectdata, inuse) == 72, "connectdata.inuse");
_Static_assert(offsetof(struct connectdata, connection_id) == 80, "connectdata.connection_id");
_Static_assert(offsetof(struct connectdata, dns_entry) == 88, "connectdata.dns_entry");
_Static_assert(offsetof(struct connectdata, ip_addr) == 96, "connectdata.ip_addr");
_Static_assert(offsetof(struct connectdata, tempaddr) == 104, "connectdata.tempaddr");
_Static_assert(offsetof(struct connectdata, ip_addr_str) == 120, "connectdata.ip_addr_str");
_Static_assert(offsetof(struct connectdata, scope_id) == 168, "connectdata.scope_id");
_Static_assert(offsetof(struct connectdata, socktype) == 172, "connectdata.socktype");
_Static_assert(offsetof(struct connectdata, host) == 176, "connectdata.host");
_Static_assert(offsetof(struct connectdata, proxy) == 208, "connectdata.proxy");
_Static_assert(offsetof(struct connectdata, port) == 240, "connectdata.port");
_Static_assert(offsetof(struct connectdata, remote_port) == 248, "connectdata.remote_port");
_Static_assert(offsetof(struct connectdata, primary_ip) == 252, "connectdata.primary_ip");
_Static_assert(offsetof(struct connectdata, primary_port) == 304, "connectdata.primary_port");
_Static_assert(offsetof(struct connectdata, local_ip) == 312, "connectdata.local_ip");
_Static_assert(offsetof(struct connectdata, local_port) == 360, "connectdata.local_port");
_Static_assert(offsetof(struct connectdata, user) == 368, "connectdata.user");
_Static_assert(offsetof(struct connectdata, passwd) == 376, "connectdata.passwd");
_Static_assert(offsetof(struct connectdata, options) == 384, "connectdata.options");
_Static_assert(offsetof(struct connectdata, xoauth2_bearer) == 392, "connectdata.xoauth2_bearer");
_Static_assert(offsetof(struct connectdata, proxyuser) == 400, "connectdata.proxyuser");
_Static_assert(offsetof(struct connectdata, proxypasswd) == 408, "connectdata.proxypasswd");
_Static_assert(offsetof(struct connectdata, proxytype) == 416, "connectdata.proxytype");
_Static_assert(offsetof(struct connectdata, httpversion) == 420, "connectdata.httpversion");
_Static_assert(offsetof(struct connectdata, rtspversion) == 424, "connectdata.rtspversion");
_Static_assert(offsetof(struct connectdata, now) == 432, "connectdata.now");
_Static_assert(offsetof(struct connectdata, created) == 448, "connectdata.created");
_Static_assert(offsetof(struct connectdata, sock) == 464, "connectdata.sock");
_Static_assert(offsetof(struct connectdata, tempsock) == 472, "connectdata.tempsock");
_Static_assert(offsetof(struct connectdata, sock_accepted) == 480, "connectdata.sock_accepted");
_Static_assert(offsetof(struct connectdata, recv) == 488, "connectdata.recv");
_Static_assert(offsetof(struct connectdata, send) == 504, "connectdata.send");
_Static_assert(offsetof(struct connectdata, ssl) == 520, "connectdata.ssl");
_Static_assert(offsetof(struct connectdata, ssl_config) == 1080, "connectdata.ssl_config");
_Static_assert(offsetof(struct connectdata, bits) == 1240, "connectdata.bits");
_Static_assert(offsetof(struct connectdata, connecttime) == 1272, "connectdata.connecttime");
_Static_assert(offsetof(struct connectdata, num_addr) == 1288, "connectdata.num_addr");
_Static_assert(offsetof(struct connectdata, timeoutms_per_addr) == 1296, "connectdata.timeoutms_per_addr");
_Static_assert(offsetof(struct connectdata, handler) == 1304, "connectdata.handler");
_Static_assert(offsetof(struct connectdata, given) == 1312, "connectdata.given");
_Static_assert(offsetof(struct connectdata, ip_version) == 1320, "connectdata.ip_version");
_Static_assert(offsetof(struct connectdata, sockfd) == 1328, "connectdata.sockfd");
_Static_assert(offsetof(struct connectdata, writesockfd) == 1332, "connectdata.writesockfd");
_Static_assert(offsetof(struct connectdata, allocptr) == 1336, "connectdata.allocptr");
_Static_assert(offsetof(struct connectdata, readchannel_inuse) == 1416, "connectdata.readchannel_inuse");
_Static_assert(offsetof(struct connectdata, writechannel_inuse) == 1417, "connectdata.writechannel_inuse");
_Static_assert(offsetof(struct connectdata, send_pipe) == 1424, "connectdata.send_pipe");
_Static_assert(offsetof(struct connectdata, recv_pipe) == 1432, "connectdata.recv_pipe");
_Static_assert(offsetof(struct connectdata, master_buffer) == 1440, "connectdata.master_buffer");
_Static_assert(offsetof(struct connectdata, read_pos) == 1448, "connectdata.read_pos");
_Static_assert(offsetof(struct connectdata, buf_len) == 1456, "connectdata.buf_len");
_Static_assert(offsetof(struct connectdata, seek_func) == 1464, "connectdata.seek_func");
_Static_assert(offsetof(struct connectdata, seek_client) == 1472, "connectdata.seek_client");
_Static_assert(offsetof(struct connectdata, fread_func) == 1480, "connectdata.fread_func");
_Static_assert(offsetof(struct connectdata, fread_in) == 1488, "connectdata.fread_in");
_Static_assert(offsetof(struct connectdata, syserr_buf) == 1496, "connectdata.syserr_buf");
_Static_assert(offsetof(struct connectdata, async) == 1752, "connectdata.async");
_Static_assert(offsetof(struct connectdata, trailer) == 1792, "connectdata.trailer");
_Static_assert(offsetof(struct connectdata, trlMax) == 1800, "connectdata.trlMax");
_Static_assert(offsetof(struct connectdata, trlPos) == 1804, "connectdata.trlPos");
_Static_assert(offsetof(struct connectdata, proto) == 1808, "connectdata.proto");
_Static_assert(offsetof(struct connectdata, cselect_bits) == 2032, "connectdata.cselect_bits");
_Static_assert(offsetof(struct connectdata, waitfor) == 2036, "connectdata.waitfor");
_Static_assert(offsetof(struct connectdata, verifypeer) == 2040, "connectdata.verifypeer");
_Static_assert(offsetof(struct connectdata, verifyhost) == 2041, "connectdata.verifyhost");
_Static_assert(offsetof(struct connectdata, localdev) == 2048, "connectdata.localdev");
_Static_assert(offsetof(struct connectdata, localport) == 2056, "connectdata.localport");
_Static_assert(offsetof(struct connectdata, localportrange) == 2060, "connectdata.localportrange");
_Static_assert(offsetof(struct connectdata, tunnel_state) == 2064, "connectdata.tunnel_state");
_Static_assert(offsetof(struct connectdata, bundle) == 2072, "connectdata.bundle");
_Static_assert(offsetof(struct connectdata, negnpn) == 2080, "connectdata.negnpn");

/* Added for conncache.c: struct SessionHandle and its by-value closure. */
struct CookieInfo;
struct Curl_multi;
struct Curl_share;
struct curl_hash;
struct curl_httppost;
struct curl_ssl_session;

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:1651 */
struct Names {
  struct curl_hash *hostcache; /* 0x0 */
  enum { HCACHE_NONE = 0, HCACHE_GLOBAL = 1, HCACHE_MULTI = 2, HCACHE_SHARED = 3 } hostcachetype; /* 0x8 */
};

_Static_assert(sizeof(struct Names) == 16, "Names size");
_Static_assert(offsetof(struct Names, hostcache) == 0, "Names.hostcache");
_Static_assert(offsetof(struct Names, hostcachetype) == 8, "Names.hostcachetype");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:612 */
enum expect100 {
  EXP100_SEND_DATA = 0,
  EXP100_AWAITING_CONTINUE = 1,
  EXP100_SENDING_REQUEST = 2,
  EXP100_FAILED = 3
};

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:620 */
enum upgrade101 {
  UPGR101_INIT = 0,
  UPGR101_REQUESTED = 1,
  UPGR101_RECEIVED = 2,
  UPGR101_WORKING = 3
};

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:634 */
struct SingleRequest {
  curl_off_t size; /* 0x0 */
  curl_off_t *bytecountp; /* 0x8 */
  curl_off_t maxdownload; /* 0x10 */
  curl_off_t *writebytecountp; /* 0x18 */
  curl_off_t bytecount; /* 0x20 */
  curl_off_t writebytecount; /* 0x28 */
  long int headerbytecount; /* 0x30 */
  long int deductheadercount; /* 0x38 */
  struct timeval start; /* 0x40 */
  struct timeval now; /* 0x50 */
  _Bool header; /* 0x60 */
  enum { HEADER_NORMAL = 0, HEADER_PARTHEADER = 1, HEADER_ALLBAD = 2 } badheader; /* 0x64 */
  int headerline; /* 0x68 */
  char *hbufp; /* 0x70 */
  size_t hbuflen; /* 0x78 */
  char *str; /* 0x80 */
  char *str_start; /* 0x88 */
  char *end_ptr; /* 0x90 */
  char *p; /* 0x98 */
  _Bool content_range; /* 0xa0 */
  curl_off_t offset; /* 0xa8 */
  int httpcode; /* 0xb0 */
  struct timeval start100; /* 0xb8 */
  enum expect100 exp100; /* 0xc8 */
  enum upgrade101 upgr101; /* 0xcc */
  int auto_decoding; /* 0xd0 */
  time_t timeofdoc; /* 0xd8 */
  long int bodywrites; /* 0xe0 */
  char *buf; /* 0xe8 */
  char *uploadbuf; /* 0xf0 */
  curl_socket_t maxfd; /* 0xf8 */
  int keepon; /* 0xfc */
  _Bool upload_done; /* 0x100 */
  _Bool ignorebody; /* 0x101 */
  _Bool ignorecl; /* 0x102 */
  char *location; /* 0x108 */
  char *newurl; /* 0x110 */
  ssize_t upload_present; /* 0x118 */
  char *upload_fromhere; /* 0x120 */
  _Bool chunk; /* 0x128 */
  _Bool upload_chunky; /* 0x129 */
  _Bool getheader; /* 0x12a */
  _Bool forbidchunk; /* 0x12b */
  void *protop; /* 0x130 */
};

_Static_assert(sizeof(struct SingleRequest) == 312, "SingleRequest size");
_Static_assert(offsetof(struct SingleRequest, size) == 0, "SingleRequest.size");
_Static_assert(offsetof(struct SingleRequest, bytecountp) == 8, "SingleRequest.bytecountp");
_Static_assert(offsetof(struct SingleRequest, maxdownload) == 16, "SingleRequest.maxdownload");
_Static_assert(offsetof(struct SingleRequest, writebytecountp) == 24, "SingleRequest.writebytecountp");
_Static_assert(offsetof(struct SingleRequest, bytecount) == 32, "SingleRequest.bytecount");
_Static_assert(offsetof(struct SingleRequest, writebytecount) == 40, "SingleRequest.writebytecount");
_Static_assert(offsetof(struct SingleRequest, headerbytecount) == 48, "SingleRequest.headerbytecount");
_Static_assert(offsetof(struct SingleRequest, deductheadercount) == 56, "SingleRequest.deductheadercount");
_Static_assert(offsetof(struct SingleRequest, start) == 64, "SingleRequest.start");
_Static_assert(offsetof(struct SingleRequest, now) == 80, "SingleRequest.now");
_Static_assert(offsetof(struct SingleRequest, header) == 96, "SingleRequest.header");
_Static_assert(offsetof(struct SingleRequest, badheader) == 100, "SingleRequest.badheader");
_Static_assert(offsetof(struct SingleRequest, headerline) == 104, "SingleRequest.headerline");
_Static_assert(offsetof(struct SingleRequest, hbufp) == 112, "SingleRequest.hbufp");
_Static_assert(offsetof(struct SingleRequest, hbuflen) == 120, "SingleRequest.hbuflen");
_Static_assert(offsetof(struct SingleRequest, str) == 128, "SingleRequest.str");
_Static_assert(offsetof(struct SingleRequest, str_start) == 136, "SingleRequest.str_start");
_Static_assert(offsetof(struct SingleRequest, end_ptr) == 144, "SingleRequest.end_ptr");
_Static_assert(offsetof(struct SingleRequest, p) == 152, "SingleRequest.p");
_Static_assert(offsetof(struct SingleRequest, content_range) == 160, "SingleRequest.content_range");
_Static_assert(offsetof(struct SingleRequest, offset) == 168, "SingleRequest.offset");
_Static_assert(offsetof(struct SingleRequest, httpcode) == 176, "SingleRequest.httpcode");
_Static_assert(offsetof(struct SingleRequest, start100) == 184, "SingleRequest.start100");
_Static_assert(offsetof(struct SingleRequest, exp100) == 200, "SingleRequest.exp100");
_Static_assert(offsetof(struct SingleRequest, upgr101) == 204, "SingleRequest.upgr101");
_Static_assert(offsetof(struct SingleRequest, auto_decoding) == 208, "SingleRequest.auto_decoding");
_Static_assert(offsetof(struct SingleRequest, timeofdoc) == 216, "SingleRequest.timeofdoc");
_Static_assert(offsetof(struct SingleRequest, bodywrites) == 224, "SingleRequest.bodywrites");
_Static_assert(offsetof(struct SingleRequest, buf) == 232, "SingleRequest.buf");
_Static_assert(offsetof(struct SingleRequest, uploadbuf) == 240, "SingleRequest.uploadbuf");
_Static_assert(offsetof(struct SingleRequest, maxfd) == 248, "SingleRequest.maxfd");
_Static_assert(offsetof(struct SingleRequest, keepon) == 252, "SingleRequest.keepon");
_Static_assert(offsetof(struct SingleRequest, upload_done) == 256, "SingleRequest.upload_done");
_Static_assert(offsetof(struct SingleRequest, ignorebody) == 257, "SingleRequest.ignorebody");
_Static_assert(offsetof(struct SingleRequest, ignorecl) == 258, "SingleRequest.ignorecl");
_Static_assert(offsetof(struct SingleRequest, location) == 264, "SingleRequest.location");
_Static_assert(offsetof(struct SingleRequest, newurl) == 272, "SingleRequest.newurl");
_Static_assert(offsetof(struct SingleRequest, upload_present) == 280, "SingleRequest.upload_present");
_Static_assert(offsetof(struct SingleRequest, upload_fromhere) == 288, "SingleRequest.upload_fromhere");
_Static_assert(offsetof(struct SingleRequest, chunk) == 296, "SingleRequest.chunk");
_Static_assert(offsetof(struct SingleRequest, upload_chunky) == 297, "SingleRequest.upload_chunky");
_Static_assert(offsetof(struct SingleRequest, getheader) == 298, "SingleRequest.getheader");
_Static_assert(offsetof(struct SingleRequest, forbidchunk) == 299, "SingleRequest.forbidchunk");
_Static_assert(offsetof(struct SingleRequest, protop) == 304, "SingleRequest.protop");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:1180 */
typedef enum {
  HTTPREQ_NONE = 0,
  HTTPREQ_GET = 1,
  HTTPREQ_POST = 2,
  HTTPREQ_POST_FORM = 3,
  HTTPREQ_PUT = 4,
  HTTPREQ_HEAD = 5,
  HTTPREQ_CUSTOM = 6,
  HTTPREQ_LAST = 7
} Curl_HttpReq;

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:1196 */
typedef enum {
  RTSPREQ_NONE = 0,
  RTSPREQ_OPTIONS = 1,
  RTSPREQ_DESCRIBE = 2,
  RTSPREQ_ANNOUNCE = 3,
  RTSPREQ_SETUP = 4,
  RTSPREQ_PLAY = 5,
  RTSPREQ_PAUSE = 6,
  RTSPREQ_TEARDOWN = 7,
  RTSPREQ_GET_PARAMETER = 8,
  RTSPREQ_SET_PARAMETER = 9,
  RTSPREQ_RECORD = 10,
  RTSPREQ_RECEIVE = 11,
  RTSPREQ_LAST = 12
} Curl_RtspReq;

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:1439 */
struct UserDefined {
  FILE *err; /* 0x0 */
  void *debugdata; /* 0x8 */
  char *errorbuffer; /* 0x10 */
  long int proxyport; /* 0x18 */
  void *out; /* 0x20 */
  void *in; /* 0x28 */
  void *writeheader; /* 0x30 */
  void *rtp_out; /* 0x38 */
  long int use_port; /* 0x40 */
  long unsigned int httpauth; /* 0x48 */
  long unsigned int proxyauth; /* 0x50 */
  _Bool proxyautoconfig; /* 0x58 */
  _Bool disablensd; /* 0x59 */
  _Bool disablecache; /* 0x5a */
  long int followlocation; /* 0x60 */
  long int maxredirs; /* 0x68 */
  int keep_post; /* 0x70 */
  _Bool free_referer; /* 0x74 */
  void *postfields; /* 0x78 */
  curl_seek_callback seek_func; /* 0x80 */
  curl_off_t postfieldsize; /* 0x88 */
  unsigned short localport; /* 0x90 */
  int localportrange; /* 0x94 */
  curl_write_callback fwrite_func; /* 0x98 */
  curl_write_callback fwrite_header; /* 0xa0 */
  curl_write_callback fwrite_rtp; /* 0xa8 */
  curl_read_callback fread_func; /* 0xb0 */
  int is_fread_set; /* 0xb8 */
  int is_fwrite_set; /* 0xbc */
  curl_progress_callback fprogress; /* 0xc0 */
  curl_xferinfo_callback fxferinfo; /* 0xc8 */
  curl_debug_callback fdebug; /* 0xd0 */
  curl_ioctl_callback ioctl_func; /* 0xd8 */
  curl_sockopt_callback fsockopt; /* 0xe0 */
  void *sockopt_client; /* 0xe8 */
  curl_opensocket_callback fopensocket; /* 0xf0 */
  void *opensocket_client; /* 0xf8 */
  curl_closesocket_callback fclosesocket; /* 0x100 */
  void *closesocket_client; /* 0x108 */
  void *seek_client; /* 0x110 */
  curl_conv_callback convfromnetwork; /* 0x118 */
  curl_conv_callback convtonetwork; /* 0x120 */
  curl_conv_callback convfromutf8; /* 0x128 */
  void *progress_client; /* 0x130 */
  void *ioctl_client; /* 0x138 */
  long int timeout; /* 0x140 */
  long int connecttimeout; /* 0x148 */
  long int accepttimeout; /* 0x150 */
  long int server_response_timeout; /* 0x158 */
  long int tftp_blksize; /* 0x160 */
  curl_off_t filesize; /* 0x168 */
  long int low_speed_limit; /* 0x170 */
  long int low_speed_time; /* 0x178 */
  curl_off_t max_send_speed; /* 0x180 */
  curl_off_t max_recv_speed; /* 0x188 */
  curl_off_t set_resume_from; /* 0x190 */
  struct curl_slist *headers; /* 0x198 */
  struct curl_slist *proxyheaders; /* 0x1a0 */
  struct curl_httppost *httppost; /* 0x1a8 */
  _Bool sep_headers; /* 0x1b0 */
  _Bool cookiesession; /* 0x1b1 */
  _Bool crlf; /* 0x1b2 */
  struct curl_slist *quote; /* 0x1b8 */
  struct curl_slist *postquote; /* 0x1c0 */
  struct curl_slist *prequote; /* 0x1c8 */
  struct curl_slist *source_quote; /* 0x1d0 */
  struct curl_slist *source_prequote; /* 0x1d8 */
  struct curl_slist *source_postquote; /* 0x1e0 */
  struct curl_slist *telnet_options; /* 0x1e8 */
  struct curl_slist *resolve; /* 0x1f0 */
  curl_TimeCond timecondition; /* 0x1f8 */
  time_t timevalue; /* 0x200 */
  Curl_HttpReq httpreq; /* 0x208 */
  long int httpversion; /* 0x210 */
  struct ssl_config_data ssl; /* 0x218 */
  curl_proxytype proxytype; /* 0x2b8 */
  long int dns_cache_timeout; /* 0x2c0 */
  long int buffer_size; /* 0x2c8 */
  void *private_data; /* 0x2d0 */
  struct curl_slist *http200aliases; /* 0x2d8 */
  long int ipver; /* 0x2e0 */
  curl_off_t max_filesize; /* 0x2e8 */
  curl_ftpfile ftp_filemethod; /* 0x2f0 */
  int ftp_create_missing_dirs; /* 0x2f4 */
  curl_sshkeycallback ssh_keyfunc; /* 0x2f8 */
  void *ssh_keyfunc_userp; /* 0x300 */
  _Bool printhost; /* 0x308 */
  _Bool get_filetime; /* 0x309 */
  _Bool tunnel_thru_httpproxy; /* 0x30a */
  _Bool prefer_ascii; /* 0x30b */
  _Bool ftp_append; /* 0x30c */
  _Bool ftp_list_only; /* 0x30d */
  _Bool ftp_use_port; /* 0x30e */
  _Bool hide_progress; /* 0x30f */
  _Bool http_fail_on_error; /* 0x310 */
  _Bool http_follow_location; /* 0x311 */
  _Bool http_transfer_encoding; /* 0x312 */
  _Bool http_disable_hostname_check_before_authentication; /* 0x313 */
  _Bool include_header; /* 0x314 */
  _Bool http_set_referer; /* 0x315 */
  _Bool http_auto_referer; /* 0x316 */
  _Bool opt_no_body; /* 0x317 */
  _Bool set_port; /* 0x318 */
  _Bool upload; /* 0x319 */
  enum CURL_NETRC_OPTION use_netrc; /* 0x31c */
  _Bool verbose; /* 0x320 */
  _Bool krb; /* 0x321 */
  _Bool reuse_forbid; /* 0x322 */
  _Bool reuse_fresh; /* 0x323 */
  _Bool ftp_use_epsv; /* 0x324 */
  _Bool ftp_use_eprt; /* 0x325 */
  _Bool ftp_use_pret; /* 0x326 */
  curl_usessl use_ssl; /* 0x328 */
  curl_ftpauth ftpsslauth; /* 0x32c */
  curl_ftpccc ftp_ccc; /* 0x330 */
  _Bool no_signal; /* 0x334 */
  _Bool global_dns_cache; /* 0x335 */
  _Bool tcp_nodelay; /* 0x336 */
  _Bool ignorecl; /* 0x337 */
  _Bool ftp_skip_ip; /* 0x338 */
  _Bool connect_only; /* 0x339 */
  _Bool ssl_enable_beast; /* 0x33a */
  long int ssh_auth_types; /* 0x340 */
  _Bool http_te_skip; /* 0x348 */
  _Bool http_ce_skip; /* 0x349 */
  long int new_file_perms; /* 0x350 */
  long int new_directory_perms; /* 0x358 */
  _Bool proxy_transfer_mode; /* 0x360 */
  char * str[42]; /* 0x368 */
  unsigned int scope_id; /* 0x4b8 */
  long int allowed_protocols; /* 0x4c0 */
  long int redir_protocols; /* 0x4c8 */
  struct curl_slist *mail_rcpt; /* 0x4d0 */
  _Bool sasl_ir; /* 0x4d8 */
  Curl_RtspReq rtspreq; /* 0x4dc */
  long int rtspversion; /* 0x4e0 */
  _Bool wildcardmatch; /* 0x4e8 */
  curl_chunk_bgn_callback chunk_bgn; /* 0x4f0 */
  curl_chunk_end_callback chunk_end; /* 0x4f8 */
  curl_fnmatch_callback fnmatch; /* 0x500 */
  void *fnmatch_data; /* 0x508 */
  long int gssapi_delegation; /* 0x510 */
  _Bool tcp_keepalive; /* 0x518 */
  long int tcp_keepidle; /* 0x520 */
  long int tcp_keepintvl; /* 0x528 */
  size_t maxconnects; /* 0x530 */
  _Bool ssl_enable_npn; /* 0x538 */
  _Bool ssl_enable_alpn; /* 0x539 */
  _Bool path_as_is; /* 0x53a */
  long int expect_100_timeout; /* 0x540 */
};

_Static_assert(sizeof(struct UserDefined) == 1352, "UserDefined size");
_Static_assert(offsetof(struct UserDefined, err) == 0, "UserDefined.err");
_Static_assert(offsetof(struct UserDefined, debugdata) == 8, "UserDefined.debugdata");
_Static_assert(offsetof(struct UserDefined, errorbuffer) == 16, "UserDefined.errorbuffer");
_Static_assert(offsetof(struct UserDefined, proxyport) == 24, "UserDefined.proxyport");
_Static_assert(offsetof(struct UserDefined, out) == 32, "UserDefined.out");
_Static_assert(offsetof(struct UserDefined, in) == 40, "UserDefined.in");
_Static_assert(offsetof(struct UserDefined, writeheader) == 48, "UserDefined.writeheader");
_Static_assert(offsetof(struct UserDefined, rtp_out) == 56, "UserDefined.rtp_out");
_Static_assert(offsetof(struct UserDefined, use_port) == 64, "UserDefined.use_port");
_Static_assert(offsetof(struct UserDefined, httpauth) == 72, "UserDefined.httpauth");
_Static_assert(offsetof(struct UserDefined, proxyauth) == 80, "UserDefined.proxyauth");
_Static_assert(offsetof(struct UserDefined, proxyautoconfig) == 88, "UserDefined.proxyautoconfig");
_Static_assert(offsetof(struct UserDefined, disablensd) == 89, "UserDefined.disablensd");
_Static_assert(offsetof(struct UserDefined, disablecache) == 90, "UserDefined.disablecache");
_Static_assert(offsetof(struct UserDefined, followlocation) == 96, "UserDefined.followlocation");
_Static_assert(offsetof(struct UserDefined, maxredirs) == 104, "UserDefined.maxredirs");
_Static_assert(offsetof(struct UserDefined, keep_post) == 112, "UserDefined.keep_post");
_Static_assert(offsetof(struct UserDefined, free_referer) == 116, "UserDefined.free_referer");
_Static_assert(offsetof(struct UserDefined, postfields) == 120, "UserDefined.postfields");
_Static_assert(offsetof(struct UserDefined, seek_func) == 128, "UserDefined.seek_func");
_Static_assert(offsetof(struct UserDefined, postfieldsize) == 136, "UserDefined.postfieldsize");
_Static_assert(offsetof(struct UserDefined, localport) == 144, "UserDefined.localport");
_Static_assert(offsetof(struct UserDefined, localportrange) == 148, "UserDefined.localportrange");
_Static_assert(offsetof(struct UserDefined, fwrite_func) == 152, "UserDefined.fwrite_func");
_Static_assert(offsetof(struct UserDefined, fwrite_header) == 160, "UserDefined.fwrite_header");
_Static_assert(offsetof(struct UserDefined, fwrite_rtp) == 168, "UserDefined.fwrite_rtp");
_Static_assert(offsetof(struct UserDefined, fread_func) == 176, "UserDefined.fread_func");
_Static_assert(offsetof(struct UserDefined, is_fread_set) == 184, "UserDefined.is_fread_set");
_Static_assert(offsetof(struct UserDefined, is_fwrite_set) == 188, "UserDefined.is_fwrite_set");
_Static_assert(offsetof(struct UserDefined, fprogress) == 192, "UserDefined.fprogress");
_Static_assert(offsetof(struct UserDefined, fxferinfo) == 200, "UserDefined.fxferinfo");
_Static_assert(offsetof(struct UserDefined, fdebug) == 208, "UserDefined.fdebug");
_Static_assert(offsetof(struct UserDefined, ioctl_func) == 216, "UserDefined.ioctl_func");
_Static_assert(offsetof(struct UserDefined, fsockopt) == 224, "UserDefined.fsockopt");
_Static_assert(offsetof(struct UserDefined, sockopt_client) == 232, "UserDefined.sockopt_client");
_Static_assert(offsetof(struct UserDefined, fopensocket) == 240, "UserDefined.fopensocket");
_Static_assert(offsetof(struct UserDefined, opensocket_client) == 248, "UserDefined.opensocket_client");
_Static_assert(offsetof(struct UserDefined, fclosesocket) == 256, "UserDefined.fclosesocket");
_Static_assert(offsetof(struct UserDefined, closesocket_client) == 264, "UserDefined.closesocket_client");
_Static_assert(offsetof(struct UserDefined, seek_client) == 272, "UserDefined.seek_client");
_Static_assert(offsetof(struct UserDefined, convfromnetwork) == 280, "UserDefined.convfromnetwork");
_Static_assert(offsetof(struct UserDefined, convtonetwork) == 288, "UserDefined.convtonetwork");
_Static_assert(offsetof(struct UserDefined, convfromutf8) == 296, "UserDefined.convfromutf8");
_Static_assert(offsetof(struct UserDefined, progress_client) == 304, "UserDefined.progress_client");
_Static_assert(offsetof(struct UserDefined, ioctl_client) == 312, "UserDefined.ioctl_client");
_Static_assert(offsetof(struct UserDefined, timeout) == 320, "UserDefined.timeout");
_Static_assert(offsetof(struct UserDefined, connecttimeout) == 328, "UserDefined.connecttimeout");
_Static_assert(offsetof(struct UserDefined, accepttimeout) == 336, "UserDefined.accepttimeout");
_Static_assert(offsetof(struct UserDefined, server_response_timeout) == 344, "UserDefined.server_response_timeout");
_Static_assert(offsetof(struct UserDefined, tftp_blksize) == 352, "UserDefined.tftp_blksize");
_Static_assert(offsetof(struct UserDefined, filesize) == 360, "UserDefined.filesize");
_Static_assert(offsetof(struct UserDefined, low_speed_limit) == 368, "UserDefined.low_speed_limit");
_Static_assert(offsetof(struct UserDefined, low_speed_time) == 376, "UserDefined.low_speed_time");
_Static_assert(offsetof(struct UserDefined, max_send_speed) == 384, "UserDefined.max_send_speed");
_Static_assert(offsetof(struct UserDefined, max_recv_speed) == 392, "UserDefined.max_recv_speed");
_Static_assert(offsetof(struct UserDefined, set_resume_from) == 400, "UserDefined.set_resume_from");
_Static_assert(offsetof(struct UserDefined, headers) == 408, "UserDefined.headers");
_Static_assert(offsetof(struct UserDefined, proxyheaders) == 416, "UserDefined.proxyheaders");
_Static_assert(offsetof(struct UserDefined, httppost) == 424, "UserDefined.httppost");
_Static_assert(offsetof(struct UserDefined, sep_headers) == 432, "UserDefined.sep_headers");
_Static_assert(offsetof(struct UserDefined, cookiesession) == 433, "UserDefined.cookiesession");
_Static_assert(offsetof(struct UserDefined, crlf) == 434, "UserDefined.crlf");
_Static_assert(offsetof(struct UserDefined, quote) == 440, "UserDefined.quote");
_Static_assert(offsetof(struct UserDefined, postquote) == 448, "UserDefined.postquote");
_Static_assert(offsetof(struct UserDefined, prequote) == 456, "UserDefined.prequote");
_Static_assert(offsetof(struct UserDefined, source_quote) == 464, "UserDefined.source_quote");
_Static_assert(offsetof(struct UserDefined, source_prequote) == 472, "UserDefined.source_prequote");
_Static_assert(offsetof(struct UserDefined, source_postquote) == 480, "UserDefined.source_postquote");
_Static_assert(offsetof(struct UserDefined, telnet_options) == 488, "UserDefined.telnet_options");
_Static_assert(offsetof(struct UserDefined, resolve) == 496, "UserDefined.resolve");
_Static_assert(offsetof(struct UserDefined, timecondition) == 504, "UserDefined.timecondition");
_Static_assert(offsetof(struct UserDefined, timevalue) == 512, "UserDefined.timevalue");
_Static_assert(offsetof(struct UserDefined, httpreq) == 520, "UserDefined.httpreq");
_Static_assert(offsetof(struct UserDefined, httpversion) == 528, "UserDefined.httpversion");
_Static_assert(offsetof(struct UserDefined, ssl) == 536, "UserDefined.ssl");
_Static_assert(offsetof(struct UserDefined, proxytype) == 696, "UserDefined.proxytype");
_Static_assert(offsetof(struct UserDefined, dns_cache_timeout) == 704, "UserDefined.dns_cache_timeout");
_Static_assert(offsetof(struct UserDefined, buffer_size) == 712, "UserDefined.buffer_size");
_Static_assert(offsetof(struct UserDefined, private_data) == 720, "UserDefined.private_data");
_Static_assert(offsetof(struct UserDefined, http200aliases) == 728, "UserDefined.http200aliases");
_Static_assert(offsetof(struct UserDefined, ipver) == 736, "UserDefined.ipver");
_Static_assert(offsetof(struct UserDefined, max_filesize) == 744, "UserDefined.max_filesize");
_Static_assert(offsetof(struct UserDefined, ftp_filemethod) == 752, "UserDefined.ftp_filemethod");
_Static_assert(offsetof(struct UserDefined, ftp_create_missing_dirs) == 756, "UserDefined.ftp_create_missing_dirs");
_Static_assert(offsetof(struct UserDefined, ssh_keyfunc) == 760, "UserDefined.ssh_keyfunc");
_Static_assert(offsetof(struct UserDefined, ssh_keyfunc_userp) == 768, "UserDefined.ssh_keyfunc_userp");
_Static_assert(offsetof(struct UserDefined, printhost) == 776, "UserDefined.printhost");
_Static_assert(offsetof(struct UserDefined, get_filetime) == 777, "UserDefined.get_filetime");
_Static_assert(offsetof(struct UserDefined, tunnel_thru_httpproxy) == 778, "UserDefined.tunnel_thru_httpproxy");
_Static_assert(offsetof(struct UserDefined, prefer_ascii) == 779, "UserDefined.prefer_ascii");
_Static_assert(offsetof(struct UserDefined, ftp_append) == 780, "UserDefined.ftp_append");
_Static_assert(offsetof(struct UserDefined, ftp_list_only) == 781, "UserDefined.ftp_list_only");
_Static_assert(offsetof(struct UserDefined, ftp_use_port) == 782, "UserDefined.ftp_use_port");
_Static_assert(offsetof(struct UserDefined, hide_progress) == 783, "UserDefined.hide_progress");
_Static_assert(offsetof(struct UserDefined, http_fail_on_error) == 784, "UserDefined.http_fail_on_error");
_Static_assert(offsetof(struct UserDefined, http_follow_location) == 785, "UserDefined.http_follow_location");
_Static_assert(offsetof(struct UserDefined, http_transfer_encoding) == 786, "UserDefined.http_transfer_encoding");
_Static_assert(offsetof(struct UserDefined, http_disable_hostname_check_before_authentication) == 787, "UserDefined.http_disable_hostname_check_before_authentication");
_Static_assert(offsetof(struct UserDefined, include_header) == 788, "UserDefined.include_header");
_Static_assert(offsetof(struct UserDefined, http_set_referer) == 789, "UserDefined.http_set_referer");
_Static_assert(offsetof(struct UserDefined, http_auto_referer) == 790, "UserDefined.http_auto_referer");
_Static_assert(offsetof(struct UserDefined, opt_no_body) == 791, "UserDefined.opt_no_body");
_Static_assert(offsetof(struct UserDefined, set_port) == 792, "UserDefined.set_port");
_Static_assert(offsetof(struct UserDefined, upload) == 793, "UserDefined.upload");
_Static_assert(offsetof(struct UserDefined, use_netrc) == 796, "UserDefined.use_netrc");
_Static_assert(offsetof(struct UserDefined, verbose) == 800, "UserDefined.verbose");
_Static_assert(offsetof(struct UserDefined, krb) == 801, "UserDefined.krb");
_Static_assert(offsetof(struct UserDefined, reuse_forbid) == 802, "UserDefined.reuse_forbid");
_Static_assert(offsetof(struct UserDefined, reuse_fresh) == 803, "UserDefined.reuse_fresh");
_Static_assert(offsetof(struct UserDefined, ftp_use_epsv) == 804, "UserDefined.ftp_use_epsv");
_Static_assert(offsetof(struct UserDefined, ftp_use_eprt) == 805, "UserDefined.ftp_use_eprt");
_Static_assert(offsetof(struct UserDefined, ftp_use_pret) == 806, "UserDefined.ftp_use_pret");
_Static_assert(offsetof(struct UserDefined, use_ssl) == 808, "UserDefined.use_ssl");
_Static_assert(offsetof(struct UserDefined, ftpsslauth) == 812, "UserDefined.ftpsslauth");
_Static_assert(offsetof(struct UserDefined, ftp_ccc) == 816, "UserDefined.ftp_ccc");
_Static_assert(offsetof(struct UserDefined, no_signal) == 820, "UserDefined.no_signal");
_Static_assert(offsetof(struct UserDefined, global_dns_cache) == 821, "UserDefined.global_dns_cache");
_Static_assert(offsetof(struct UserDefined, tcp_nodelay) == 822, "UserDefined.tcp_nodelay");
_Static_assert(offsetof(struct UserDefined, ignorecl) == 823, "UserDefined.ignorecl");
_Static_assert(offsetof(struct UserDefined, ftp_skip_ip) == 824, "UserDefined.ftp_skip_ip");
_Static_assert(offsetof(struct UserDefined, connect_only) == 825, "UserDefined.connect_only");
_Static_assert(offsetof(struct UserDefined, ssl_enable_beast) == 826, "UserDefined.ssl_enable_beast");
_Static_assert(offsetof(struct UserDefined, ssh_auth_types) == 832, "UserDefined.ssh_auth_types");
_Static_assert(offsetof(struct UserDefined, http_te_skip) == 840, "UserDefined.http_te_skip");
_Static_assert(offsetof(struct UserDefined, http_ce_skip) == 841, "UserDefined.http_ce_skip");
_Static_assert(offsetof(struct UserDefined, new_file_perms) == 848, "UserDefined.new_file_perms");
_Static_assert(offsetof(struct UserDefined, new_directory_perms) == 856, "UserDefined.new_directory_perms");
_Static_assert(offsetof(struct UserDefined, proxy_transfer_mode) == 864, "UserDefined.proxy_transfer_mode");
_Static_assert(offsetof(struct UserDefined, str) == 872, "UserDefined.str");
_Static_assert(offsetof(struct UserDefined, scope_id) == 1208, "UserDefined.scope_id");
_Static_assert(offsetof(struct UserDefined, allowed_protocols) == 1216, "UserDefined.allowed_protocols");
_Static_assert(offsetof(struct UserDefined, redir_protocols) == 1224, "UserDefined.redir_protocols");
_Static_assert(offsetof(struct UserDefined, mail_rcpt) == 1232, "UserDefined.mail_rcpt");
_Static_assert(offsetof(struct UserDefined, sasl_ir) == 1240, "UserDefined.sasl_ir");
_Static_assert(offsetof(struct UserDefined, rtspreq) == 1244, "UserDefined.rtspreq");
_Static_assert(offsetof(struct UserDefined, rtspversion) == 1248, "UserDefined.rtspversion");
_Static_assert(offsetof(struct UserDefined, wildcardmatch) == 1256, "UserDefined.wildcardmatch");
_Static_assert(offsetof(struct UserDefined, chunk_bgn) == 1264, "UserDefined.chunk_bgn");
_Static_assert(offsetof(struct UserDefined, chunk_end) == 1272, "UserDefined.chunk_end");
_Static_assert(offsetof(struct UserDefined, fnmatch) == 1280, "UserDefined.fnmatch");
_Static_assert(offsetof(struct UserDefined, fnmatch_data) == 1288, "UserDefined.fnmatch_data");
_Static_assert(offsetof(struct UserDefined, gssapi_delegation) == 1296, "UserDefined.gssapi_delegation");
_Static_assert(offsetof(struct UserDefined, tcp_keepalive) == 1304, "UserDefined.tcp_keepalive");
_Static_assert(offsetof(struct UserDefined, tcp_keepidle) == 1312, "UserDefined.tcp_keepidle");
_Static_assert(offsetof(struct UserDefined, tcp_keepintvl) == 1320, "UserDefined.tcp_keepintvl");
_Static_assert(offsetof(struct UserDefined, maxconnects) == 1328, "UserDefined.maxconnects");
_Static_assert(offsetof(struct UserDefined, ssl_enable_npn) == 1336, "UserDefined.ssl_enable_npn");
_Static_assert(offsetof(struct UserDefined, ssl_enable_alpn) == 1337, "UserDefined.ssl_enable_alpn");
_Static_assert(offsetof(struct UserDefined, path_as_is) == 1338, "UserDefined.path_as_is");
_Static_assert(offsetof(struct UserDefined, expect_100_timeout) == 1344, "UserDefined.expect_100_timeout");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:1344 */
struct DynamicStatic {
  char *url; /* 0x0 */
  _Bool url_alloc; /* 0x8 */
  char *referer; /* 0x10 */
  _Bool referer_alloc; /* 0x18 */
  struct curl_slist *cookielist; /* 0x20 */
  struct curl_slist *resolve; /* 0x28 */
};

_Static_assert(sizeof(struct DynamicStatic) == 48, "DynamicStatic size");
_Static_assert(offsetof(struct DynamicStatic, url) == 0, "DynamicStatic.url");
_Static_assert(offsetof(struct DynamicStatic, url_alloc) == 8, "DynamicStatic.url_alloc");
_Static_assert(offsetof(struct DynamicStatic, referer) == 16, "DynamicStatic.referer");
_Static_assert(offsetof(struct DynamicStatic, referer_alloc) == 24, "DynamicStatic.referer_alloc");
_Static_assert(offsetof(struct DynamicStatic, cookielist) == 32, "DynamicStatic.cookielist");
_Static_assert(offsetof(struct DynamicStatic, resolve) == 40, "DynamicStatic.resolve");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:1134 */
struct Progress {
  long int lastshow; /* 0x0 */
  curl_off_t size_dl; /* 0x8 */
  curl_off_t size_ul; /* 0x10 */
  curl_off_t downloaded; /* 0x18 */
  curl_off_t uploaded; /* 0x20 */
  curl_off_t current_speed; /* 0x28 */
  _Bool callback; /* 0x30 */
  int width; /* 0x34 */
  int flags; /* 0x38 */
  double timespent; /* 0x40 */
  curl_off_t dlspeed; /* 0x48 */
  curl_off_t ulspeed; /* 0x50 */
  double t_nslookup; /* 0x58 */
  double t_connect; /* 0x60 */
  double t_appconnect; /* 0x68 */
  double t_pretransfer; /* 0x70 */
  double t_starttransfer; /* 0x78 */
  double t_redirect; /* 0x80 */
  struct timeval start; /* 0x88 */
  struct timeval t_startsingle; /* 0x98 */
  struct timeval t_startop; /* 0xa8 */
  struct timeval t_acceptdata; /* 0xb8 */
  curl_off_t speeder[6]; /* 0xc8 */
  struct timeval speeder_time[6]; /* 0xf8 */
  int speeder_c; /* 0x158 */
};

_Static_assert(sizeof(struct Progress) == 352, "Progress size");
_Static_assert(offsetof(struct Progress, lastshow) == 0, "Progress.lastshow");
_Static_assert(offsetof(struct Progress, size_dl) == 8, "Progress.size_dl");
_Static_assert(offsetof(struct Progress, size_ul) == 16, "Progress.size_ul");
_Static_assert(offsetof(struct Progress, downloaded) == 24, "Progress.downloaded");
_Static_assert(offsetof(struct Progress, uploaded) == 32, "Progress.uploaded");
_Static_assert(offsetof(struct Progress, current_speed) == 40, "Progress.current_speed");
_Static_assert(offsetof(struct Progress, callback) == 48, "Progress.callback");
_Static_assert(offsetof(struct Progress, width) == 52, "Progress.width");
_Static_assert(offsetof(struct Progress, flags) == 56, "Progress.flags");
_Static_assert(offsetof(struct Progress, timespent) == 64, "Progress.timespent");
_Static_assert(offsetof(struct Progress, dlspeed) == 72, "Progress.dlspeed");
_Static_assert(offsetof(struct Progress, ulspeed) == 80, "Progress.ulspeed");
_Static_assert(offsetof(struct Progress, t_nslookup) == 88, "Progress.t_nslookup");
_Static_assert(offsetof(struct Progress, t_connect) == 96, "Progress.t_connect");
_Static_assert(offsetof(struct Progress, t_appconnect) == 104, "Progress.t_appconnect");
_Static_assert(offsetof(struct Progress, t_pretransfer) == 112, "Progress.t_pretransfer");
_Static_assert(offsetof(struct Progress, t_starttransfer) == 120, "Progress.t_starttransfer");
_Static_assert(offsetof(struct Progress, t_redirect) == 128, "Progress.t_redirect");
_Static_assert(offsetof(struct Progress, start) == 136, "Progress.start");
_Static_assert(offsetof(struct Progress, t_startsingle) == 152, "Progress.t_startsingle");
_Static_assert(offsetof(struct Progress, t_startop) == 168, "Progress.t_startop");
_Static_assert(offsetof(struct Progress, t_acceptdata) == 184, "Progress.t_acceptdata");
_Static_assert(offsetof(struct Progress, speeder) == 200, "Progress.speeder");
_Static_assert(offsetof(struct Progress, speeder_time) == 248, "Progress.speeder_time");
_Static_assert(offsetof(struct Progress, speeder_c) == 344, "Progress.speeder_c");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:396 */
struct digestdata {
  char *nonce; /* 0x0 */
  char *cnonce; /* 0x8 */
  char *realm; /* 0x10 */
  int algo; /* 0x18 */
  _Bool stale; /* 0x1c */
  char *opaque; /* 0x20 */
  char *qop; /* 0x28 */
  char *algorithm; /* 0x30 */
  int nc; /* 0x38 */
};

_Static_assert(sizeof(struct digestdata) == 64, "digestdata size");
_Static_assert(offsetof(struct digestdata, nonce) == 0, "digestdata.nonce");
_Static_assert(offsetof(struct digestdata, cnonce) == 8, "digestdata.cnonce");
_Static_assert(offsetof(struct digestdata, realm) == 16, "digestdata.realm");
_Static_assert(offsetof(struct digestdata, algo) == 24, "digestdata.algo");
_Static_assert(offsetof(struct digestdata, stale) == 28, "digestdata.stale");
_Static_assert(offsetof(struct digestdata, opaque) == 32, "digestdata.opaque");
_Static_assert(offsetof(struct digestdata, qop) == 40, "digestdata.qop");
_Static_assert(offsetof(struct digestdata, algorithm) == 48, "digestdata.algorithm");
_Static_assert(offsetof(struct digestdata, nc) == 56, "digestdata.nc");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:1209 */
struct auth {
  long unsigned int want; /* 0x0 */
  long unsigned int picked; /* 0x8 */
  long unsigned int avail; /* 0x10 */
  _Bool done; /* 0x18 */
  _Bool multi; /* 0x19 */
  _Bool iestyle; /* 0x1a */
};

_Static_assert(sizeof(struct auth) == 32, "auth size");
_Static_assert(offsetof(struct auth, want) == 0, "auth.want");
_Static_assert(offsetof(struct auth, picked) == 8, "auth.picked");
_Static_assert(offsetof(struct auth, avail) == 16, "auth.avail");
_Static_assert(offsetof(struct auth, done) == 24, "auth.done");
_Static_assert(offsetof(struct auth, multi) == 25, "auth.multi");
_Static_assert(offsetof(struct auth, iestyle) == 26, "auth.iestyle");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:1223 */
struct UrlState {
  struct conncache *conn_cache; /* 0x0 */
  _Bool multi_owned_by_easy; /* 0x8 */
  struct timeval keeps_speed; /* 0x10 */
  struct connectdata *lastconnect; /* 0x20 */
  char *headerbuff; /* 0x28 */
  size_t headersize; /* 0x30 */
  char buffer[16385]; /* 0x38 */
  char uploadbuffer[16385]; /* 0x4039 */
  curl_off_t current_speed; /* 0x8040 */
  _Bool this_is_a_follow; /* 0x8048 */
  char *first_host; /* 0x8050 */
  struct curl_ssl_session *session; /* 0x8058 */
  long int sessionage; /* 0x8060 */
  char *tempwrite; /* 0x8068 */
  size_t tempwritesize; /* 0x8070 */
  int tempwritetype; /* 0x8078 */
  char *scratch; /* 0x8080 */
  _Bool errorbuf; /* 0x8088 */
  int os_errno; /* 0x808c */
  _Bool allow_port; /* 0x8090 */
  struct digestdata digest; /* 0x8098 */
  struct digestdata proxydigest; /* 0x80d8 */
  struct auth authhost; /* 0x8118 */
  struct auth authproxy; /* 0x8138 */
  _Bool authproblem; /* 0x8158 */
  void *resolver; /* 0x8160 */
  struct timeval expiretime; /* 0x8168 */
  struct Curl_tree timenode; /* 0x8178 */
  struct curl_llist *timeoutlist; /* 0x81a8 */
  char *most_recent_ftp_entrypath; /* 0x81b0 */
  _Bool ftp_trying_alternative; /* 0x81b8 */
  int httpversion; /* 0x81bc */
  _Bool expect100header; /* 0x81c0 */
  _Bool pipe_broke; /* 0x81c1 */
  _Bool prev_block_had_trailing_cr; /* 0x81c2 */
  curl_off_t crlf_conversions; /* 0x81c8 */
  char *pathbuffer; /* 0x81d0 */
  char *path; /* 0x81d8 */
  _Bool slash_removed; /* 0x81e0 */
  _Bool use_range; /* 0x81e1 */
  _Bool rangestringalloc; /* 0x81e2 */
  char *range; /* 0x81e8 */
  curl_off_t resume_from; /* 0x81f0 */
  long int rtsp_next_client_CSeq; /* 0x81f8 */
  long int rtsp_next_server_CSeq; /* 0x8200 */
  long int rtsp_CSeq_recv; /* 0x8208 */
  curl_off_t infilesize; /* 0x8210 */
};

_Static_assert(sizeof(struct UrlState) == 33304, "UrlState size");
_Static_assert(offsetof(struct UrlState, conn_cache) == 0, "UrlState.conn_cache");
_Static_assert(offsetof(struct UrlState, multi_owned_by_easy) == 8, "UrlState.multi_owned_by_easy");
_Static_assert(offsetof(struct UrlState, keeps_speed) == 16, "UrlState.keeps_speed");
_Static_assert(offsetof(struct UrlState, lastconnect) == 32, "UrlState.lastconnect");
_Static_assert(offsetof(struct UrlState, headerbuff) == 40, "UrlState.headerbuff");
_Static_assert(offsetof(struct UrlState, headersize) == 48, "UrlState.headersize");
_Static_assert(offsetof(struct UrlState, buffer) == 56, "UrlState.buffer");
_Static_assert(offsetof(struct UrlState, uploadbuffer) == 16441, "UrlState.uploadbuffer");
_Static_assert(offsetof(struct UrlState, current_speed) == 32832, "UrlState.current_speed");
_Static_assert(offsetof(struct UrlState, this_is_a_follow) == 32840, "UrlState.this_is_a_follow");
_Static_assert(offsetof(struct UrlState, first_host) == 32848, "UrlState.first_host");
_Static_assert(offsetof(struct UrlState, session) == 32856, "UrlState.session");
_Static_assert(offsetof(struct UrlState, sessionage) == 32864, "UrlState.sessionage");
_Static_assert(offsetof(struct UrlState, tempwrite) == 32872, "UrlState.tempwrite");
_Static_assert(offsetof(struct UrlState, tempwritesize) == 32880, "UrlState.tempwritesize");
_Static_assert(offsetof(struct UrlState, tempwritetype) == 32888, "UrlState.tempwritetype");
_Static_assert(offsetof(struct UrlState, scratch) == 32896, "UrlState.scratch");
_Static_assert(offsetof(struct UrlState, errorbuf) == 32904, "UrlState.errorbuf");
_Static_assert(offsetof(struct UrlState, os_errno) == 32908, "UrlState.os_errno");
_Static_assert(offsetof(struct UrlState, allow_port) == 32912, "UrlState.allow_port");
_Static_assert(offsetof(struct UrlState, digest) == 32920, "UrlState.digest");
_Static_assert(offsetof(struct UrlState, proxydigest) == 32984, "UrlState.proxydigest");
_Static_assert(offsetof(struct UrlState, authhost) == 33048, "UrlState.authhost");
_Static_assert(offsetof(struct UrlState, authproxy) == 33080, "UrlState.authproxy");
_Static_assert(offsetof(struct UrlState, authproblem) == 33112, "UrlState.authproblem");
_Static_assert(offsetof(struct UrlState, resolver) == 33120, "UrlState.resolver");
_Static_assert(offsetof(struct UrlState, expiretime) == 33128, "UrlState.expiretime");
_Static_assert(offsetof(struct UrlState, timenode) == 33144, "UrlState.timenode");
_Static_assert(offsetof(struct UrlState, timeoutlist) == 33192, "UrlState.timeoutlist");
_Static_assert(offsetof(struct UrlState, most_recent_ftp_entrypath) == 33200, "UrlState.most_recent_ftp_entrypath");
_Static_assert(offsetof(struct UrlState, ftp_trying_alternative) == 33208, "UrlState.ftp_trying_alternative");
_Static_assert(offsetof(struct UrlState, httpversion) == 33212, "UrlState.httpversion");
_Static_assert(offsetof(struct UrlState, expect100header) == 33216, "UrlState.expect100header");
_Static_assert(offsetof(struct UrlState, pipe_broke) == 33217, "UrlState.pipe_broke");
_Static_assert(offsetof(struct UrlState, prev_block_had_trailing_cr) == 33218, "UrlState.prev_block_had_trailing_cr");
_Static_assert(offsetof(struct UrlState, crlf_conversions) == 33224, "UrlState.crlf_conversions");
_Static_assert(offsetof(struct UrlState, pathbuffer) == 33232, "UrlState.pathbuffer");
_Static_assert(offsetof(struct UrlState, path) == 33240, "UrlState.path");
_Static_assert(offsetof(struct UrlState, slash_removed) == 33248, "UrlState.slash_removed");
_Static_assert(offsetof(struct UrlState, use_range) == 33249, "UrlState.use_range");
_Static_assert(offsetof(struct UrlState, rangestringalloc) == 33250, "UrlState.rangestringalloc");
_Static_assert(offsetof(struct UrlState, range) == 33256, "UrlState.range");
_Static_assert(offsetof(struct UrlState, resume_from) == 33264, "UrlState.resume_from");
_Static_assert(offsetof(struct UrlState, rtsp_next_client_CSeq) == 33272, "UrlState.rtsp_next_client_CSeq");
_Static_assert(offsetof(struct UrlState, rtsp_next_server_CSeq) == 33280, "UrlState.rtsp_next_server_CSeq");
_Static_assert(offsetof(struct UrlState, rtsp_CSeq_recv) == 33288, "UrlState.rtsp_CSeq_recv");
_Static_assert(offsetof(struct UrlState, infilesize) == 33296, "UrlState.infilesize");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:1096 */
struct PureInfo {
  int httpcode; /* 0x0 */
  int httpproxycode; /* 0x4 */
  int httpversion; /* 0x8 */
  long int filetime; /* 0x10 */
  _Bool timecond; /* 0x18 */
  long int header_size; /* 0x20 */
  long int request_size; /* 0x28 */
  long unsigned int proxyauthavail; /* 0x30 */
  long unsigned int httpauthavail; /* 0x38 */
  long int numconnects; /* 0x40 */
  char *contenttype; /* 0x48 */
  char *wouldredirect; /* 0x50 */
  char conn_primary_ip[46]; /* 0x58 */
  long int conn_primary_port; /* 0x88 */
  char conn_local_ip[46]; /* 0x90 */
  long int conn_local_port; /* 0xc0 */
  struct curl_certinfo certs; /* 0xc8 */
};

_Static_assert(sizeof(struct PureInfo) == 216, "PureInfo size");
_Static_assert(offsetof(struct PureInfo, httpcode) == 0, "PureInfo.httpcode");
_Static_assert(offsetof(struct PureInfo, httpproxycode) == 4, "PureInfo.httpproxycode");
_Static_assert(offsetof(struct PureInfo, httpversion) == 8, "PureInfo.httpversion");
_Static_assert(offsetof(struct PureInfo, filetime) == 16, "PureInfo.filetime");
_Static_assert(offsetof(struct PureInfo, timecond) == 24, "PureInfo.timecond");
_Static_assert(offsetof(struct PureInfo, header_size) == 32, "PureInfo.header_size");
_Static_assert(offsetof(struct PureInfo, request_size) == 40, "PureInfo.request_size");
_Static_assert(offsetof(struct PureInfo, proxyauthavail) == 48, "PureInfo.proxyauthavail");
_Static_assert(offsetof(struct PureInfo, httpauthavail) == 56, "PureInfo.httpauthavail");
_Static_assert(offsetof(struct PureInfo, numconnects) == 64, "PureInfo.numconnects");
_Static_assert(offsetof(struct PureInfo, contenttype) == 72, "PureInfo.contenttype");
_Static_assert(offsetof(struct PureInfo, wouldredirect) == 80, "PureInfo.wouldredirect");
_Static_assert(offsetof(struct PureInfo, conn_primary_ip) == 88, "PureInfo.conn_primary_ip");
_Static_assert(offsetof(struct PureInfo, conn_primary_port) == 136, "PureInfo.conn_primary_port");
_Static_assert(offsetof(struct PureInfo, conn_local_ip) == 144, "PureInfo.conn_local_ip");
_Static_assert(offsetof(struct PureInfo, conn_local_port) == 192, "PureInfo.conn_local_port");
_Static_assert(offsetof(struct PureInfo, certs) == 200, "PureInfo.certs");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:1671 */
struct SessionHandle {
  struct SessionHandle *next; /* 0x0 */
  struct SessionHandle *prev; /* 0x8 */
  struct connectdata *easy_conn; /* 0x10 */
  CURLMstate mstate; /* 0x18 */
  CURLcode result; /* 0x1c */
  struct Curl_message msg; /* 0x20 */
  curl_socket_t sockets[5]; /* 0x38 */
  int numsocks; /* 0x4c */
  struct Names dns; /* 0x50 */
  struct Curl_multi *multi; /* 0x60 */
  struct Curl_multi *multi_easy; /* 0x68 */
  struct Curl_share *share; /* 0x70 */
  struct SingleRequest req; /* 0x78 */
  struct UserDefined set; /* 0x1b0 */
  struct DynamicStatic change; /* 0x6f8 */
  struct CookieInfo *cookies; /* 0x728 */
  struct Progress progress; /* 0x730 */
  struct UrlState state; /* 0x890 */
  struct WildcardData wildcard; /* 0x8aa8 */
  struct PureInfo info; /* 0x8ae0 */
  struct curl_tlssessioninfo tsi; /* 0x8bb8 */
  unsigned int magic; /* 0x8bc8 */
};

_Static_assert(sizeof(struct SessionHandle) == 35792, "SessionHandle size");
_Static_assert(offsetof(struct SessionHandle, next) == 0, "SessionHandle.next");
_Static_assert(offsetof(struct SessionHandle, prev) == 8, "SessionHandle.prev");
_Static_assert(offsetof(struct SessionHandle, easy_conn) == 16, "SessionHandle.easy_conn");
_Static_assert(offsetof(struct SessionHandle, mstate) == 24, "SessionHandle.mstate");
_Static_assert(offsetof(struct SessionHandle, result) == 28, "SessionHandle.result");
_Static_assert(offsetof(struct SessionHandle, msg) == 32, "SessionHandle.msg");
_Static_assert(offsetof(struct SessionHandle, sockets) == 56, "SessionHandle.sockets");
_Static_assert(offsetof(struct SessionHandle, numsocks) == 76, "SessionHandle.numsocks");
_Static_assert(offsetof(struct SessionHandle, dns) == 80, "SessionHandle.dns");
_Static_assert(offsetof(struct SessionHandle, multi) == 96, "SessionHandle.multi");
_Static_assert(offsetof(struct SessionHandle, multi_easy) == 104, "SessionHandle.multi_easy");
_Static_assert(offsetof(struct SessionHandle, share) == 112, "SessionHandle.share");
_Static_assert(offsetof(struct SessionHandle, req) == 120, "SessionHandle.req");
_Static_assert(offsetof(struct SessionHandle, set) == 432, "SessionHandle.set");
_Static_assert(offsetof(struct SessionHandle, change) == 1784, "SessionHandle.change");
_Static_assert(offsetof(struct SessionHandle, cookies) == 1832, "SessionHandle.cookies");
_Static_assert(offsetof(struct SessionHandle, progress) == 1840, "SessionHandle.progress");
_Static_assert(offsetof(struct SessionHandle, state) == 2192, "SessionHandle.state");
_Static_assert(offsetof(struct SessionHandle, wildcard) == 35496, "SessionHandle.wildcard");
_Static_assert(offsetof(struct SessionHandle, info) == 35552, "SessionHandle.info");
_Static_assert(offsetof(struct SessionHandle, tsi) == 35768, "SessionHandle.tsi");
_Static_assert(offsetof(struct SessionHandle, magic) == 35784, "SessionHandle.magic");

/* D:/home/teamcity2/work/sdk/Externals/curl/lib/urldata.h:386 */
struct curl_ssl_session {
  char *name; /* 0x0 */
  void *sessionid; /* 0x8 */
  size_t idsize; /* 0x10 */
  long age; /* 0x18 */
  int remote_port; /* 0x20 */
  struct ssl_config_data ssl_config; /* 0x28 */
};

_Static_assert(sizeof(struct curl_ssl_session) == 200, "curl_ssl_session size");
_Static_assert(offsetof(struct curl_ssl_session, ssl_config) == 40, "curl_ssl_session.ssl_config");


/* Added for url.c: the protocol handler, the set.str[] index and the
 * protocol/handler flag bits (urldata.h:609-830, 1366; curl.h CURLPROTO_*). */
typedef CURLcode (*Curl_do_more_func)(struct connectdata *, int *);
typedef CURLcode (*Curl_done_func)(struct connectdata *, CURLcode, _Bool);

struct Curl_handler {
  const char *scheme;
  CURLcode (*setup_connection)(struct connectdata *);
  CURLcode (*do_it)(struct connectdata *, _Bool *done);
  Curl_done_func done;
  Curl_do_more_func do_more;
  CURLcode (*connect_it)(struct connectdata *, _Bool *done);
  CURLcode (*connecting)(struct connectdata *, _Bool *done);
  CURLcode (*doing)(struct connectdata *, _Bool *done);
  int (*proto_getsock)(struct connectdata *conn, curl_socket_t *socks,
                       int numsocks);
  int (*doing_getsock)(struct connectdata *conn, curl_socket_t *socks,
                       int numsocks);
  int (*domore_getsock)(struct connectdata *conn, curl_socket_t *socks,
                        int numsocks);
  int (*perform_getsock)(const struct connectdata *conn,
                         curl_socket_t *socks, int numsocks);
  CURLcode (*disconnect)(struct connectdata *, _Bool dead_connection);
  CURLcode (*readwrite)(struct SessionHandle *data, struct connectdata *conn,
                        ssize_t *nread, _Bool *readmore);
  long defport;
  unsigned int protocol;
  unsigned int flags;
};

#define PROTOPT_NONE 0
#define PROTOPT_SSL (1<<0)
#define PROTOPT_DUAL (1<<1)
#define PROTOPT_CLOSEACTION (1<<2)
#define PROTOPT_DIRLOCK (1<<3)
#define PROTOPT_NONETWORK (1<<4)
#define PROTOPT_NEEDSPWD (1<<5)
#define PROTOPT_NOURLQUERY (1<<6)
#define PROTOPT_CREDSPERREQUEST (1<<7)

#define CURLPROTO_HTTP   (1<<0)
#define CURLPROTO_HTTPS  (1<<1)
#define CURLPROTO_FTP    (1<<2)
#define CURLPROTO_FTPS   (1<<3)
#define CURLPROTO_SCP    (1<<4)
#define CURLPROTO_SFTP   (1<<5)
#define CURLPROTO_TELNET (1<<6)
#define CURLPROTO_LDAP   (1<<7)
#define CURLPROTO_LDAPS  (1<<8)
#define CURLPROTO_DICT   (1<<9)
#define CURLPROTO_FILE   (1<<10)
#define CURLPROTO_TFTP   (1<<11)
#define CURLPROTO_IMAP   (1<<12)
#define CURLPROTO_IMAPS  (1<<13)
#define CURLPROTO_POP3   (1<<14)
#define CURLPROTO_POP3S  (1<<15)
#define CURLPROTO_SMTP   (1<<16)
#define CURLPROTO_SMTPS  (1<<17)
#define CURLPROTO_RTSP   (1<<18)
#define CURLPROTO_RTMP   (1<<19)
#define CURLPROTO_RTMPT  (1<<20)
#define CURLPROTO_RTMPE  (1<<21)
#define CURLPROTO_RTMPTE (1<<22)
#define CURLPROTO_RTMPS  (1<<23)
#define CURLPROTO_RTMPTS (1<<24)
#define CURLPROTO_GOPHER (1<<25)
#define CURLPROTO_SMB    (1<<26)
#define CURLPROTO_SMBS   (1<<27)
#define CURLPROTO_ALL    (~0)

enum dupstring {
  STRING_CERT, STRING_CERT_TYPE, STRING_COOKIE, STRING_COOKIEJAR,
  STRING_CUSTOMREQUEST, STRING_DEVICE, STRING_ENCODING, STRING_FTP_ACCOUNT,
  STRING_FTP_ALTERNATIVE_TO_USER, STRING_FTPPORT, STRING_KEY,
  STRING_KEY_PASSWD, STRING_KEY_TYPE, STRING_KRB_LEVEL, STRING_NETRC_FILE,
  STRING_PROXY, STRING_SET_RANGE, STRING_SET_REFERER, STRING_SET_URL,
  STRING_SSL_CAPATH, STRING_SSL_CAFILE, STRING_SSL_PINNEDPUBLICKEY,
  STRING_SSL_CIPHER_LIST, STRING_SSL_EGDSOCKET, STRING_SSL_RANDOM_FILE,
  STRING_USERAGENT, STRING_SSL_CRLFILE, STRING_SSL_ISSUERCERT,
  STRING_USERNAME, STRING_PASSWORD, STRING_OPTIONS, STRING_PROXYUSERNAME,
  STRING_PROXYPASSWORD, STRING_NOPROXY, STRING_RTSP_SESSION_ID,
  STRING_RTSP_STREAM_URI, STRING_RTSP_TRANSPORT, STRING_MAIL_FROM,
  STRING_MAIL_AUTH, STRING_BEARER, STRING_LASTZEROTERMINATED,
  STRING_COPYPOSTFIELDS, STRING_LAST
};

/* Added for http.c (urldata.h:32-33, 66, 204, 222-223, 573-583, 593-597). */
#define PORT_HTTP 80
#define PORT_HTTPS 443
#define PROTO_FAMILY_HTTP (CURLPROTO_HTTP|CURLPROTO_HTTPS)
#define CURLMAX(x,y) ((x)>(y)?(x):(y))
#define FIRSTSOCKET     0
#define SECONDARYSOCKET 1
#define KEEP_NONE  0
#define KEEP_RECV  (1<<0)
#define KEEP_SEND  (1<<1)
#define KEEP_RECV_HOLD (1<<2)
#define KEEP_SEND_HOLD (1<<3)
#define KEEP_RECV_PAUSE (1<<4)
#define KEEP_SEND_PAUSE (1<<5)
#define KEEP_RECVBITS (KEEP_RECV | KEEP_RECV_HOLD | KEEP_RECV_PAUSE)
#define KEEP_SENDBITS (KEEP_SEND | KEEP_SEND_HOLD | KEEP_SEND_PAUSE)
#define IDENTITY 0
#define DEFLATE 1
#define GZIP 2
#define COMPRESS 3
#define BUFSIZE CURL_MAX_WRITE_SIZE

#endif /* D3_RECOVERED_EXTERNALS_CURL_LIB_URLDATA_H */
