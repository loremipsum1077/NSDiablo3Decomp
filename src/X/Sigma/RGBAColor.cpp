#include "RGBAColor.h"

// Values read from the original's data (0x7100e5de88). The original also defines
// GAMMA_FACTOR, INV_GAMMA_FACTOR, crgbaTan and crgba*Value (lines 22-54); the linker
// dropped them, so their values are unknown and they are left out here.
// RGBAColorValue::Normalize and DWORDToRGBAColor (lines 62, 89) have no copy in the binary.

const RGBAColor crgbaWhite = {255, 255, 255, 255};
const RGBAColor crgbaGray = {136, 136, 136, 255};
const RGBAColor crgbaRed = {255, 0, 0, 255};
const RGBAColor crgbaGreen = {0, 255, 0, 255};
const RGBAColor crgbaBlue = {0, 0, 255, 255};
const RGBAColor crgbaYellow = {255, 255, 0, 255};
const RGBAColor crgbaOrange = {255, 127, 0, 255};
const RGBAColor crgbaPurple = {255, 0, 255, 255};
const RGBAColor crgbaGold = {199, 179, 119, 255};
const RGBAColor crgbaUIGold = {205, 157, 81, 255};
const RGBAColor crgbaBlack = {0, 0, 0, 255};
const RGBAColor crgbaUIOrange = {255, 105, 30, 255};
const RGBAColor unk_7100ed3648 = {0, 255, 225, 255};  // 2.7.7: used by ItemGetDisplayNameColor
const RGBAColor crgbaDisabled = {102, 102, 102, 136};

const RGBAColorValue crgbavalNull = {0.0f, 0.0f, 0.0f, 0.0f};
const RGBAColorValue crgbavalOne = {1.0f, 1.0f, 1.0f, 1.0f};
