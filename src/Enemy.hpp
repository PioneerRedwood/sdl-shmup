//------------------------------------------------------------------------------
// File: Enemy.hpp
// Author: Chris Redwood
// Created: 2024-11-04
// License: MIT License
//------------------------------------------------------------------------------

#pragma once 

#include <SDL.h>
#include "GameObject.hpp"

namespace shmup {

enum EnemyState {
  EnemyStateIdle,
  EnemyStateHit,
  EnemyStateMove,
};

class Enemy : public GameObject {
public:
  Enemy();

  ~Enemy();

  /// @brief 충돌 시 콜백
  void onCollided(const GameObject& target) override;

  void updateCollidePosition(SDL_FPoint pos);

  static void setColliderRadius(float radius);

public:
  const SDL_FPoint* debugColliderPoints() const { return m_debugColliderPoints; }

  /// @brief 주어진 수치만큼 디버그 콜라이더 포인트 이동
  void moveYPosColliderPoints(float deltaY);

  void speed(float speed) { m_speed = speed; }
  
  float speed() const { return m_speed; }

  void state(EnemyState state) { m_state = state; }

  EnemyState state() const { return m_state; }

private:
  SDL_FPoint* m_debugColliderPoints = nullptr;

  float m_speed = 0.0f;

  EnemyState m_state = EnemyStateIdle;

  static float s_colliderRadius;
};

}