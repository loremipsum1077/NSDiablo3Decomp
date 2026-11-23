// Check for RGBAColor::Set(const RGBAColorValue &) (X/Sigma/RGBAColor.inl:111).
// The binary's only out-of-line copy (0x7100190810) sits among X/Client/Weather.cpp's
// functions: it is Weather.cpp's linkonce copy of the inline, not code from
// RGBAColor.cpp. Taking its address here makes this file emit it so try_file.py can
// compare it until Weather.cpp is written.
#include "X/Sigma/RGBAColor.h"

extern void (RGBAColor::*pfnSetFromValue)(const RGBAColorValue&);
void (RGBAColor::*pfnSetFromValue)(const RGBAColorValue&) = &RGBAColor::Set;
