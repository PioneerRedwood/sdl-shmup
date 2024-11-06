//------------------------------------------------------------------------------
// File: Enemy.hpp
// Author: Chris Redwood
// Created: 2024-11-04
// License: MIT License
//------------------------------------------------------------------------------

#pragma once 

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

  void setCollider(float x, float y, float radius) override;

  /// @brief 적의 모든 콜라이더의 반지름은 공유됨
  static void setColliderRadius(float radius);

  Vector2 nextPos(double delta) const;

public:
  Vector2* debugColliderPoints() const { return m_debugColliderPoints; }

  /// @brief 주어진 수치만큼 디버그 콜라이더 포인트 이동
  void moveDebugColliderPoints(const Vector2& pos);

  void speed(float speed) { m_speed = speed; }
  
  float speed() const { return m_speed; }

  void state(EnemyState state) { m_state = state; }

  EnemyState state() const { return m_state; }

  void destination(Vector2 pos) { m_destination = pos; }

  const Vector2& destination() { return m_destination; }

private:
  Vector2* m_debugColliderPoints = nullptr;

  float m_speed = 0.0f;

  EnemyState m_state = EnemyStateIdle;

  static float s_enemyColliderRadius;

  Vector2 m_destination;
};

}