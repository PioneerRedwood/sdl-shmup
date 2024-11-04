//------------------------------------------------------------------------------
// File: Math.cpp
// Author: Chris Redwood
// Created: 2024-11-01
// License: MIT License
//------------------------------------------------------------------------------

#include "Math.hpp"

namespace shmup {

float Math::distance(const SDL_FPoint& a, const SDL_FPoint& b) {
  return (float)std::sqrt(std::fabsf(a.x - b.x) * std::fabsf(a.x - b.x) +
                          std::fabsf(a.y - b.y) * std::fabsf(a.y - b.y));
}

void Math::createCirclePoints(SDL_FPoint* points, float x, float y,
                              float radius) {
  float angle = 0.0f;
  const float stepSize = 2.0f;
  for (int i = 0; i < 180; ++i, angle += stepSize) {
    points[i] =
        {x + std::cosf(angle) * radius, y + std::sinf(angle) * radius};
  }
}

}  // namespace shmup
