//------------------------------------------------------------------------------
// File: Enemy.cpp
// Author: Chris Redwood
// Created: 2024-11-04
// License: MIT License
//------------------------------------------------------------------------------

#include "Enemy.hpp"
#include "Math.hpp"

namespace shmup {

float Enemy::s_colliderRadius = 0.0f;

Enemy::Enemy() : GameObject() {
  m_tag = GameObjectTagEnemy;
  m_isVisible = true;
  m_state = EnemyStateIdle;
  m_debugColliderPoints = new Vector2[180];
}

Enemy::~Enemy() {
  if(m_debugColliderPoints != nullptr) {
    delete[] m_debugColliderPoints;
  }
}

void Enemy::onCollided(const GameObject& target) {
  switch (target.tag()) {
    case GameObjectTagPlayer: {
      // TODO: 사라지고 플레이어에게 데미지
      break;
    }
    case GameObjectTagBullet: {
      // TODO: 데미지 받음
      isVisible(false);
      m_state = EnemyStateHit;
      break;
    }
    default: {
      break;
    }
  }
}

/// @brief 콜라이더 설정. 이미 존재하면 주어진 값으로 기존 콜라이더 값 변경.
///  단 반지름은 전역적인 값 유지. 
void Enemy::setCollider(float x, float y, float radius) {
  if(m_collider == nullptr) {
    m_collider = new CircleCollider();
  }
  m_collider->position = { x, y };
  m_collider->radius = s_colliderRadius;
}

/// @brief 전역적으로 에너미 콜라이더의 반지름을 설정
void Enemy::setColliderRadius(float radius) {
  s_colliderRadius = radius;
}

void Enemy::moveDebugColliderPoints(const Vector2& pos) {
  Math::createCirclePoints(m_debugColliderPoints, pos.x, pos.y,
                           s_colliderRadius);
}

Vector2 Enemy::nextPos(double delta) const {
  double deltaSeconds = delta / 1000.0f;
  return {(float)(m_position.x + m_speed * deltaSeconds),
          (float)(m_position.y + m_speed * deltaSeconds)};
}

}  // namespace shmup