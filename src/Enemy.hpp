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

  /// @brief 충돌 시 호출되는 함수
  void onCollided(const GameObject& target) override;

  /// @brief 콜라이더 설정. 이미 존재하면 주어진 값으로 기존 콜라이더 값 변경.
  ///  단 반지름은 전역적인 값 유지. 
  void setCollider(float x, float y, float radius) override;

  /// @brief 전역적으로 적 콜라이더의 반지름을 설정.
  static void setColliderRadius(float radius);

  /// @brief 만약 움직이고 있다면 주어진 시간의 예상되는 콜라이더 중심 좌표 반환.
  /// 만약 움직이도록 설정되지 않았거나 목적지에 위치해 있다면 현재 위치 반환. 
  Vector2 getColliderCenterByDelta(double delta) const;

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