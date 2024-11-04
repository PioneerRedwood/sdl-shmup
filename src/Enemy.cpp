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

  m_debugColliderPoints = new SDL_FPoint[180];

  m_visible = true;

  m_state = EnemyStateMove;
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
      visible(false);
      m_state = EnemyStateHit;
      break;
    }
    default: {
      break;
    }
  }
}

void Enemy::updateCollidePosition(SDL_FPoint pos) {
  if (m_collider != nullptr && m_collider->position.x == pos.x &&
      m_collider->position.y == pos.y) {
    return;
  }

  setCollider(pos.x, pos.y, s_colliderRadius);

  // 충돌체 원형 좌표 수정
  Math::createCirclePoints(m_debugColliderPoints, pos.x, pos.y,
                           s_colliderRadius);
}

void Enemy::setColliderRadius(float radius) {
  s_colliderRadius = radius;
}

void Enemy::moveYPosColliderPoints(float deltaY) {
  for(unsigned i = 0; i < 180; ++i) {
    m_debugColliderPoints[i].y += deltaY;
  }
}

}  // namespace shmup