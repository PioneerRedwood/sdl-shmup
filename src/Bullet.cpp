//------------------------------------------------------------------------------
// File: Bullet.cpp
// Author: Chris Redwood
// Created: 2024-11-04
// License: MIT License
//------------------------------------------------------------------------------

#include "Bullet.hpp"

namespace shmup {

// 총알 관련 속성
float s_bulletSpeed = 300.0f;

Bullet::Bullet() : GameObject() {
  // 기본값 설정
  m_size = {32.0f, 32.0f};
  
  setCollider(0.0f, 0.0f, 0.0f);
  position({ 0.0f, 0.0f });
  
  m_isVisible = false;
  m_tag = GameObjectTagBullet;
  m_speed = s_bulletSpeed;
}

Bullet::~Bullet() {}

void Bullet::speed(float speed) { m_speed = speed; }

float Bullet::speed() const { return m_speed; }

void Bullet::state(BulletState state) { m_state = state; }

BulletState Bullet::state() const { return m_state; }

void Bullet::onCollided(const GameObject& target) {
  if (target.tag() == GameObjectTagEnemy) {
    m_state = BulletStateIdle;
    // TODO: 맞았을 때 처리
    m_isVisible = false;
  }
}

Vector2 Bullet::nextPos(double delta) const {
  double deltaSeconds = delta / 1000.0f;
  return {(float)(m_position.x + s_bulletSpeed * deltaSeconds),
          (float)(m_position.y + s_bulletSpeed * deltaSeconds)};
}

}  // namespace shmup