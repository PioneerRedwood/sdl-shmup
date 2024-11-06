//------------------------------------------------------------------------------
// File: Math.hpp
// Author: Chris Redwood
// Created: 2024-10-30
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include <vector>
#include <cstdlib>
#include <cmath>

namespace shmup {

struct Vector2 {
  float x;
  float y;

  Vector2();
  Vector2(float x, float y);

  Vector2 operator+(const Vector2& other) const;
  Vector2 operator*(float scalar) const;
  Vector2& operator=(const Vector2& other);
  Vector2 operator-(const Vector2& other) const;

  /// @brief 벡터의 크기 계산
  float magnitude() const;

  /// @brief 벡터 노멀라이즈 (정규화)
  Vector2 normalized() const;
};

/// @brief 간단한 수학 메서드 제공
class Math {
public:
  /// @brief 두 좌표 간의 거리
  static float distance(const Vector2& a, const Vector2& b);

  /// @brief 정해진 좌표를 중심으로 주어진 반지름으로 구성된 원 좌표를 반환 (좌표 갯수는 항상 180개로 고정)
  static void createCirclePoints(Vector2* points, float x, float y, float radius);
};


}
