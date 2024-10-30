//------------------------------------------------------------------------------
// File: Math.hpp
// Author: Chris Redwood
// Created: 2024-10-30
// License: MIT License
//------------------------------------------------------------------------------

#include <SDL.h>
#include <vector>
#include <cstdlib>
#include <cmath>

namespace shmup::math {

/// @brief 두 좌표 간의 거리
float distance(const SDL_FPoint& a, const SDL_FPoint& b) {
    return (float)std::sqrt(
        std::fabsf(a.x - b.x) * std::fabsf(a.x - b.x)
        + std::fabsf(a.y - b.y) * std::fabsf(a.y - b.y)
    );
}

/// @brief 좌표를 기준으로 정해진 스텝만큼 원에 대한 좌표 벡터를 반환. 
std::vector<SDL_FPoint> createCirclePointVector(float x, float y, float radius, int steps) {
    std::vector<SDL_FPoint> retValue;
    retValue.reserve(steps);

    float angle = 0.0f;
    float stepSize = 360.0f / steps;
    for(int i = 0; i <= steps; ++i, angle += stepSize) {
        retValue.push_back({ x + std::cosf(angle) * radius, y + std::sinf(angle) * radius });
    }
    return retValue; // copy by value?
}

}