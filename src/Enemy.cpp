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
      isVisible(false);
      m_state = EnemyStateIdle;
      position({1000.0f, 1000.0f});
      setCollider(1000.0f, 1000.0f, s_enemyColliderRadius);
      break;
    }
    case GameObjectTagBullet: {
      // TODO: 데미지 받음
      isVisible(false);
      m_state = EnemyStateHit;
      position({ 1000.0f, 1000.0f });
      setCollider(1000.0f, 1000.0f, s_enemyColliderRadius);
      break;
    }
    default: {
      break;
    }
  }
}

void Enemy::setCollider(float x, float y, float radius) {
  if(m_collider == nullptr) {
    m_collider = new CircleCollider();
  }
  m_collider->position = { x, y };
  m_collider->radius = s_enemyColliderRadius;
}

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

Vector2 Enemy::getColliderCenterByDelta(double delta) const {
  if(m_state == EnemyStateMove) {
     float magnitude = m_speed * delta;
     Vector2 pos = { m_collider->position.x, m_collider->position.y + magnitude };

    // 목적지에 도착을 했으면 목적지 위치의 충돌체 중심 좌표를 반환
    if((m_destination - pos).magnitude() <= magnitude) {
      // 목적지에 도착했다면 사라져있을 것이므로 검사하는 것은 의미가 없음
      return { -1.0f, -1.0f };
    } else {
      return pos;
    }
  }
  return m_collider->position;
}

}  // namespace shmup
