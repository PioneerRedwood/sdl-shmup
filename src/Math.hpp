//------------------------------------------------------------------------------
// File: Math.hpp
// Author: Chris Redwood
// Created: 2024-10-30
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include <SDL.h>
#include <vector>
#include <cstdlib>
#include <cmath>

namespace shmup {

/// @brief 간단한 수학 메서드 제공
class Math {
public:
  /// @brief 두 좌표 간의 거리
  static float distance(const SDL_FPoint& a, const SDL_FPoint& b);

  /// @brief 정해진 좌표를 중심으로 주어진 반지름으로 구성된 원 좌표를 반환 (좌표 갯수는 항상 180개로 고정)
  static void createCirclePoints(SDL_FPoint* points, float x, float y, float radius);
};


}
