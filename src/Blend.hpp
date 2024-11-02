//------------------------------------------------------------------------------
// File: Blend.hpp
// Author: Chris Redwood
// Created: 2024-10-29
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include <algorithm>

#include "RGBA.hpp"

namespace shmup::blend {

RGBAf convertToFloat(const RGBA& c) {
  // MSVC 사용 시 아래와 같이 구조체 초기화하는 기법은 C++20 이상부터 지원
  return RGBAf {
      .r = (float)c.r / 255.0f,
      .g = (float)c.g / 255.0f,
      .b = (float)c.b / 255.0f,
      .a = (float)c.a / 255.0f,
  };
}

uint8_t normalizeToByte(float colorValue) {
  return (uint8_t)(std::min(255.0f, colorValue * 255.0f));
}

/// Alpha blend
/// dstRGB = (srcRGB * srcA) + (dstRGB * (1-srcA))
/// dstA = srcA + (dstA * (1-srcA))
RGBA alpha(const RGBA& src, const RGBA& dst) {
  RGBA retValue = {0};
  RGBAf s = convertToFloat(src), d = convertToFloat(dst);
  const float invSrcAlpha = 1.0f - s.a;
  retValue.r = normalizeToByte(s.r * s.a + d.r * invSrcAlpha);
  retValue.g = normalizeToByte(s.g * s.a + d.g * invSrcAlpha);
  retValue.b = normalizeToByte(s.b * s.a + d.b * invSrcAlpha);
  retValue.a = normalizeToByte(s.a + (d.a * invSrcAlpha));
  return retValue;
}

/// additive blending
/// dstRGB = (srcRGB * srcA) + dstRGB
/// dstA = dstA
RGBA additive(const RGBA& src, const RGBA& dst) {
  RGBA retValue = {0};
  RGBAf s = convertToFloat(src), d = convertToFloat(dst);
  retValue.r = normalizeToByte((s.r * s.a) + d.r);
  retValue.g = normalizeToByte((s.g * s.a) + d.g);
  retValue.b = normalizeToByte((s.b * s.a) + d.b);
  retValue.a = dst.a;
  return retValue;
}

/// color multiply
/// dstRGB = (srcRGB * dstRGB) + (dstRGB * (1 - srcA))
/// dstA = dstA
RGBA multiply(const RGBA& src, const RGBA& dst) {
  RGBA retValue = {0};
  RGBAf s = convertToFloat(src), d = convertToFloat(dst);
  const float invSrcAlpha = 1.0f - s.a;
  retValue.r = normalizeToByte((s.r * d.r) + (d.r * invSrcAlpha));
  retValue.g = normalizeToByte((s.g * d.g) + (d.g * invSrcAlpha));
  retValue.b = normalizeToByte((s.b * d.b) + (d.b * invSrcAlpha));
  retValue.a = dst.a;
  return retValue;
}

}  // namespace shmup::blend