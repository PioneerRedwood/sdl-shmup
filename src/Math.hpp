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

  /// @brief 좌표를 기준으로 정해진 스텝만큼 원에 대한 좌표 벡터를 반환. 
  static void createCirclePoints(std::vector<SDL_FPoint>& points, float x, float y, float radius, int steps);
};


}
