//------------------------------------------------------------------------------
// File: Math.cpp
// Author: Chris Redwood
// Created: 2024-11-01
// License: MIT License
//------------------------------------------------------------------------------

#include "Math.hpp"

namespace shmup {

Vector2::Vector2() : x(0.0f), y(0.0f) {}

Vector2::Vector2(float x, float y) : x(x), y(y) {}

Vector2 Vector2::operator+(const Vector2& other) const {
  return { x + other.x, y + other.y };
}

Vector2 Vector2::operator*(float scalar) const {
  return { x * scalar, y * scalar };
}

Vector2& Vector2::operator=(const Vector2& other) {
  x = other.x;
  y = other.y;
  return *this;
}

Vector2 Vector2::operator-(const Vector2& other) const {
  return { x - other.x, y - other.y };
}

float Vector2::magnitude() const {
  return sqrtf(x * x + y * y);
}

Vector2 Vector2::normalized() const {
  float mag = magnitude();
  return { x / mag, y / mag };
}

float Math::distance(const Vector2& a, const Vector2& b) {
  return (float)std::sqrt(std::fabsf(a.x - b.x) * std::fabsf(a.x - b.x) +
                          std::fabsf(a.y - b.y) * std::fabsf(a.y - b.y));
}

void Math::createCirclePoints(Vector2* points, float x, float y,
                              float radius) {
  float angle = 0.0f;
  const float stepSize = 2.0f;
  for (int i = 0; i < 180; ++i, angle += stepSize) {
    points[i] =
        {x + std::cosf(angle) * radius, y + std::sinf(angle) * radius};
  }
}

}  // namespace shmup
