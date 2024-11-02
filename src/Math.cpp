//------------------------------------------------------------------------------
// File: Math.cpp
// Author: Chris Redwood
// Created: 2024-11-01
// License: MIT License
//------------------------------------------------------------------------------

#include "Math.hpp"

namespace shmup {

float Math::distance(const SDL_FPoint& a, const SDL_FPoint& b) {
    return (float)std::sqrt(
        std::fabsf(a.x - b.x) * std::fabsf(a.x - b.x)
        + std::fabsf(a.y - b.y) * std::fabsf(a.y - b.y)
    );
}

void Math::createCirclePointVector(std::vector<SDL_FPoint>& points, float x, float y, float radius, int steps) {
    points.reserve(steps);

    float angle = 0.0f;
    float stepSize = 360.0f / steps;
    for(int i = 0; i <= steps; ++i, angle += stepSize) {
        points.push_back({ x + std::cosf(angle) * radius, y + std::sinf(angle) * radius });
    }
}

}