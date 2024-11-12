//------------------------------------------------------------------------------
// File: Blend.hpp
// Author: Chris Redwood
// Created: 2024-10-29
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include <algorithm>

#include "RGBA.hpp"

namespace shmup {

struct Blend {
public:

static RGBAf convertToFloat(const RGBA& c);

static uint8_t normalizeToByte(float colorValue);

/// Alpha Blend
/// dstRGB = (srcRGB * srcA) + (dstRGB * (1-srcA))
/// dstA = srcA + (dstA * (1-srcA))
static RGBA alpha(const RGBA& src, const RGBA& dst);

/// color pre-multiplied alpha Blend
/// 소스 RGB에 이미 소스 알파값이 적용되어 있음을 전제
/// dstRGB = srcRGB + (dstRGB * (1-srcA))
/// dstA = srcA + (dstA * (1-srcA))
static RGBA premultipliedAlpha(const RGBA& src, const RGBA& dst);

/// additive Blending
/// dstRGB = (srcRGB * srcA) + dstRGB
/// dstA = dstA
static RGBA additive(const RGBA& src, const RGBA& dst);

/// color multiply
/// dstRGB = (srcRGB * dstRGB) + (dstRGB * (1 - srcA))
/// dstA = dstA
static RGBA multiply(const RGBA& src, const RGBA& dst);

}; // struct Blend

}  // namespace shmup
