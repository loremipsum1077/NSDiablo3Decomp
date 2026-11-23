// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/fmodstudiosrc11005/lowlevel_api/src/fmod_dsp.h

#pragma once

#include "fmod_common.h"

typedef struct FMOD_DSP_STATE FMOD_DSP_STATE;

typedef struct FMOD_DSP_BUFFER_ARRAY FMOD_DSP_BUFFER_ARRAY;

typedef enum {
    FMOD_DSP_PROCESS_PERFORM = 0,
    FMOD_DSP_PROCESS_QUERY = 1,
} FMOD_DSP_PROCESS_OPERATION;

typedef FMOD_RESULT (*FMOD_DSP_CREATE_CALLBACK)(FMOD_DSP_STATE *);

typedef FMOD_RESULT (*FMOD_DSP_RELEASE_CALLBACK)(FMOD_DSP_STATE *);

typedef FMOD_RESULT (*FMOD_DSP_RESET_CALLBACK)(FMOD_DSP_STATE *);

typedef FMOD_RESULT (*FMOD_DSP_READ_CALLBACK)(FMOD_DSP_STATE *, float *, float *, unsigned int, int, int *);

typedef FMOD_RESULT (*FMOD_DSP_PROCESS_CALLBACK)(FMOD_DSP_STATE *, unsigned int, const FMOD_DSP_BUFFER_ARRAY *, FMOD_DSP_BUFFER_ARRAY *, FMOD_BOOL, FMOD_DSP_PROCESS_OPERATION);

typedef FMOD_RESULT (*FMOD_DSP_SETPOSITION_CALLBACK)(FMOD_DSP_STATE *, unsigned int);

typedef FMOD_RESULT (*FMOD_DSP_SHOULDIPROCESS_CALLBACK)(FMOD_DSP_STATE *, FMOD_BOOL, unsigned int, FMOD_CHANNELMASK, int, FMOD_SPEAKERMODE);

typedef FMOD_RESULT (*FMOD_DSP_SETPARAM_FLOAT_CALLBACK)(FMOD_DSP_STATE *, int, float);

typedef FMOD_RESULT (*FMOD_DSP_SETPARAM_INT_CALLBACK)(FMOD_DSP_STATE *, int, int);

typedef FMOD_RESULT (*FMOD_DSP_SETPARAM_BOOL_CALLBACK)(FMOD_DSP_STATE *, int, FMOD_BOOL);

typedef FMOD_RESULT (*FMOD_DSP_SETPARAM_DATA_CALLBACK)(FMOD_DSP_STATE *, int, void *, unsigned int);

typedef FMOD_RESULT (*FMOD_DSP_GETPARAM_FLOAT_CALLBACK)(FMOD_DSP_STATE *, int, float *, char *);

typedef FMOD_RESULT (*FMOD_DSP_GETPARAM_INT_CALLBACK)(FMOD_DSP_STATE *, int, int *, char *);

typedef FMOD_RESULT (*FMOD_DSP_GETPARAM_BOOL_CALLBACK)(FMOD_DSP_STATE *, int, FMOD_BOOL *, char *);

typedef FMOD_RESULT (*FMOD_DSP_GETPARAM_DATA_CALLBACK)(FMOD_DSP_STATE *, int, void **, unsigned int *, char *);

typedef FMOD_RESULT (*FMOD_DSP_SYSTEM_REGISTER_CALLBACK)(FMOD_DSP_STATE *);

typedef FMOD_RESULT (*FMOD_DSP_SYSTEM_DEREGISTER_CALLBACK)(FMOD_DSP_STATE *);

typedef FMOD_RESULT (*FMOD_DSP_SYSTEM_MIX_CALLBACK)(FMOD_DSP_STATE *, int);

typedef struct FMOD_DSP_PARAMETER_DESC FMOD_DSP_PARAMETER_DESC;

struct FMOD_DSP_DESCRIPTION {  // size 0xd8
    unsigned int pluginsdkversion;  // 0x00
    char name[32];  // 0x04
    unsigned int version;  // 0x24
    int numinputbuffers;  // 0x28
    int numoutputbuffers;  // 0x2c
    FMOD_DSP_CREATE_CALLBACK create;  // 0x30
    FMOD_DSP_RELEASE_CALLBACK release;  // 0x38
    FMOD_DSP_RESET_CALLBACK reset;  // 0x40
    FMOD_DSP_READ_CALLBACK read;  // 0x48
    FMOD_DSP_PROCESS_CALLBACK process;  // 0x50
    FMOD_DSP_SETPOSITION_CALLBACK setposition;  // 0x58
    int numparameters;  // 0x60
    FMOD_DSP_PARAMETER_DESC ** paramdesc;  // 0x68
    FMOD_DSP_SETPARAM_FLOAT_CALLBACK setparameterfloat;  // 0x70
    FMOD_DSP_SETPARAM_INT_CALLBACK setparameterint;  // 0x78
    FMOD_DSP_SETPARAM_BOOL_CALLBACK setparameterbool;  // 0x80
    FMOD_DSP_SETPARAM_DATA_CALLBACK setparameterdata;  // 0x88
    FMOD_DSP_GETPARAM_FLOAT_CALLBACK getparameterfloat;  // 0x90
    FMOD_DSP_GETPARAM_INT_CALLBACK getparameterint;  // 0x98
    FMOD_DSP_GETPARAM_BOOL_CALLBACK getparameterbool;  // 0xa0
    FMOD_DSP_GETPARAM_DATA_CALLBACK getparameterdata;  // 0xa8
    FMOD_DSP_SHOULDIPROCESS_CALLBACK shouldiprocess;  // 0xb0
    void * userdata;  // 0xb8
    FMOD_DSP_SYSTEM_REGISTER_CALLBACK sys_register;  // 0xc0
    FMOD_DSP_SYSTEM_DEREGISTER_CALLBACK sys_deregister;  // 0xc8
    FMOD_DSP_SYSTEM_MIX_CALLBACK sys_mix;  // 0xd0
};
static_assert(sizeof(FMOD_DSP_DESCRIPTION) == 0xd8, "FMOD_DSP_DESCRIPTION");

typedef struct FMOD_DSP_DESCRIPTION FMOD_DSP_DESCRIPTION;

typedef struct FMOD_DSP_STATE_FUNCTIONS FMOD_DSP_STATE_FUNCTIONS;

struct FMOD_DSP_STATE {  // size 0x38
    void * instance;  // 0x00
    void * plugindata;  // 0x08
    FMOD_CHANNELMASK channelmask;  // 0x10
    FMOD_SPEAKERMODE source_speakermode;  // 0x14
    float * sidechaindata;  // 0x18
    int sidechainchannels;  // 0x20
    FMOD_DSP_STATE_FUNCTIONS * functions;  // 0x28
    int systemobject;  // 0x30
};
static_assert(sizeof(FMOD_DSP_STATE) == 0x38, "FMOD_DSP_STATE");

