/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggTheora SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE Theora SOURCE CODE IS COPYRIGHT (C) 2002-2009                *
 * by the Xiph.Org Foundation and contributors http://www.xiph.org/ *
 *                                                                  *
 ********************************************************************

  function:
    last mod: $Id: info.c 16503 2009-08-22 18:14:02Z giles $

 ********************************************************************/

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "internal.h"



/*This is more or less the same as strncasecmp, but that doesn't exist
   everywhere, and this is a fairly trivial function, so we include it.
  Note: We take advantage of the fact that we know _n is less than or equal to
   the length of at least one of the strings.*/
static int oc_tagcompare(const char *_s1,const char *_s2,int _n){
  int c;
  for(c=0;c<_n;c++){
    if(toupper(_s1[c])!=toupper(_s2[c]))return !0;
  }
  return _s1[c]!='=';
}



void th_info_init(th_info *_info){
  memset(_info,0,sizeof(*_info));
  _info->version_major=TH_VERSION_MAJOR;
  _info->version_minor=TH_VERSION_MINOR;
  _info->version_subminor=TH_VERSION_SUB;
  _info->keyframe_granule_shift=6;
}

void th_info_clear(th_info *_info){
  memset(_info,0,sizeof(*_info));
}



void th_comment_init(th_comment *_tc){
  memset(_tc,0,sizeof(*_tc));
}

void th_comment_clear(th_comment *_tc){
  if(_tc!=NULL){
    long i;
    for(i=0;i<_tc->comments;i++)_ogg_free(_tc->user_comments[i]);
    _ogg_free(_tc->user_comments);
    _ogg_free(_tc->comment_lengths);
    _ogg_free(_tc->vendor);
    memset(_tc,0,sizeof(*_tc));
  }
}
