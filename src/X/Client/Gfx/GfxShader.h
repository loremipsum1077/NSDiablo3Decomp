// Original file: X/Client/Gfx/GfxShader.h
// Minimal: only the inline helpers proven so far (GfxShaderHasConstant written from the copies
// inlined into sSetCommonVSConstants, GfxShader.cpp; GfxShaderGetConstantFastBinding from the
// copies inlined into PostFX.cpp and SubObjectGfx.cpp).

#pragma once

#include "../Definitions/ShaderCommon.h"

struct Matrix4x4;

const ShaderConstBinding * GfxShaderGetConstantBinding( const int32 nIndex );  // GfxShader.cpp:2355
const ShaderConstFastBinding * GfxShaderGetConstantFastBindings();  // GfxShader.cpp:2385
void GfxShaderSetConstant( const ShaderConstFastBinding * ptBinding, const void * lpData, const uint32 dwNumFloat4 );  // GfxShader.cpp:2399
void GfxShaderSetConstant( const ShaderConstBinding * ptBinding_, const void * lpData, const uint32 dwNumFloat4 );  // GfxShader.cpp:2429
void GfxShaderTexMatrixSet( const TextureBinding * ptTexBinding, const Matrix4x4 & m );  // GfxShader.cpp:2305

// Line 254.
inline const ShaderConstFastBinding * GfxShaderGetConstantFastBinding( const ShaderConstFastBinding * pShaderConstFastBindings, const int32 nIndex )
{
    return &pShaderConstFastBindings[nIndex];
}

// Line 268.
inline BOOL GfxShaderHasConstant( const ShaderConstFastBinding * pShaderConstFastBindings, const int32 nIndex )
{






    return pShaderConstFastBindings[nIndex].dwFlags != 0;
}
