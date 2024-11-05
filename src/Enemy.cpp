//------------------------------------------------------------------------------
// File: Enemy.cpp
// Author: Chris Redwood
// Created: 2024-11-04
// License: MIT License
//------------------------------------------------------------------------------

#include "Enemy.hpp"
#include "Math.hpp"

namespace shmup {

// 적 관련 속성
float Enemy::s_enemyColliderRadius = 0.0f;

Enemy::Enemy() : GameObject() {
  m_tag = GameObjectTagEnemy;
  m_isVisible = false;
  m_state = EnemyStateIdle;
  m_debugColliderPoints = new Vector2[180];
  
  // 콜라이더 위치는 오브젝트에 의해 변경됨
  setCollider(0.0f, 0.0f, s_enemyColliderRadius);
  position({ 0.0f, 0.0f });
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
  m_collider->radius = s_enemyColliderRadius;
}

/// @brief 전역적으로 적 콜라이더의 반지름을 설정
void Enemy::setColliderRadius(float radius) {
  s_enemyColliderRadius = radius;
}

void Enemy::moveDebugColliderPoints(const Vector2& pos) {
  Math::createCirclePoints(m_debugColliderPoints, pos.x, pos.y,
                           s_enemyColliderRadius);
}

Vector2 Enemy::nextPos(double delta) const {
  double deltaSeconds = delta / 1000.0f;
  return {(float)(m_position.x + m_speed * deltaSeconds),
          (float)(m_position.y + m_speed * deltaSeconds)};
}

}  // namespace shmup