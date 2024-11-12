//------------------------------------------------------------------------------
// File: Blend.cpp
// Author: Chris Redwood
// Created: 2024-11-12
// License: MIT License
//------------------------------------------------------------------------------

#include "Blend.hpp"

namespace shmup {

RGBAf Blend::convertToFloat(const RGBA& c) {
  // MSVC 사용 시 아래와 같이 구조체 초기화하는 기법은 C++20 이상부터 지원
  return RGBAf {
      .r = (float)c.r / 255.0f,
      .g = (float)c.g / 255.0f,
      .b = (float)c.b / 255.0f,
      .a = (float)c.a / 255.0f,
  };
}

uint8_t Blend::normalizeToByte(float colorValue) {
  return (uint8_t)(std::min(255.0f, colorValue * 255.0f));
}

/// Alpha Blend
/// dstRGB = (srcRGB * srcA) + (dstRGB * (1-srcA))
/// dstA = srcA + (dstA * (1-srcA))
RGBA Blend::alpha(const RGBA& src, const RGBA& dst) {
  RGBA retValue = {0};
  RGBAf s = convertToFloat(src), d = convertToFloat(dst);
  const float invSrcAlpha = 1.0f - s.a;
  retValue.r = normalizeToByte(s.r * s.a + d.r * invSrcAlpha);
  retValue.g = normalizeToByte(s.g * s.a + d.g * invSrcAlpha);
  retValue.b = normalizeToByte(s.b * s.a + d.b * invSrcAlpha);
  retValue.a = normalizeToByte(s.a + (d.a * invSrcAlpha));
  return retValue;
}

/// color pre-multiplied alpha Blend
/// 소스 RGB에 이미 소스 알파값이 적용되어 있음을 전제
/// dstRGB = srcRGB + (dstRGB * (1-srcA))
/// dstA = srcA + (dstA * (1-srcA))
RGBA Blend::premultipliedAlpha(const RGBA& src, const RGBA& dst) {
  RGBA retValue = {0};
  RGBAf s = convertToFloat(src), d = convertToFloat(dst);
  const float invSrcAlpha = 1.0f - s.a;
  retValue.r = normalizeToByte(s.r + d.r * invSrcAlpha);
  retValue.g = normalizeToByte(s.g + d.g * invSrcAlpha);
  retValue.b = normalizeToByte(s.b + d.b * invSrcAlpha);
  retValue.a = normalizeToByte(s.a + (d.a * invSrcAlpha));
  return retValue;
}

/// additive Blending
/// dstRGB = (srcRGB * srcA) + dstRGB
/// dstA = dstA
RGBA Blend::additive(const RGBA& src, const RGBA& dst) {
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
RGBA Blend::multiply(const RGBA& src, const RGBA& dst) {
  RGBA retValue = {0};
  RGBAf s = convertToFloat(src), d = convertToFloat(dst);
  const float invSrcAlpha = 1.0f - s.a;
  retValue.r = normalizeToByte((s.r * d.r) + (d.r * invSrcAlpha));
  retValue.g = normalizeToByte((s.g * d.g) + (d.g * invSrcAlpha));
  retValue.b = normalizeToByte((s.b * d.b) + (d.b * invSrcAlpha));
  retValue.a = dst.a;
  return retValue;
}

}  // namespace shmup
