//------------------------------------------------------------------------------
// File: Bullet.cpp
// Author: Chris Redwood
// Created: 2024-11-04
// License: MIT License
//------------------------------------------------------------------------------

#include "Bullet.hpp"

namespace shmup {

// 총알 관련 속성
float s_bulletSpeed = 0.3f;

Bullet::Bullet() : GameObject() {
  // 기본값 설정
  //m_size = {16.0f, 16.0f};
  m_size = {16.0f, 16.0f};
  
  setCollider(0.0f, 0.0f, 0.0f);
  position({ 0.0f, 0.0f });
  
  m_isVisible = false;
  m_tag = GameObjectTagBullet;
  m_speed = s_bulletSpeed;

  m_debugPoints = new Vector2[180];
}

Bullet::~Bullet() {
  if (m_debugPoints) {
    delete[] m_debugPoints;
  }
}

void Bullet::speed(float speed) { m_speed = speed; }

float Bullet::speed() const { return m_speed; }

void Bullet::state(BulletState state) { m_state = state; }

BulletState Bullet::state() const { return m_state; }

Vector2* Bullet::debugPoints() const { return m_debugPoints; }

void Bullet::onCollided(const GameObject& target) {
  if (target.tag() == GameObjectTagEnemy) {
    m_state = BulletStateIdle;
    // TODO: 맞았을 때 처리
    m_isVisible = false;

    // 먼 곳으로 옮기기
    m_position = { -1000.0f, -1000.0f };
    m_collider->position = { -1000.0f, -1000.0f };
  }
}

Vector2 Bullet::nextPos(double delta) const {
  double deltaSeconds = delta / 1000.0f;
  return {(float)(m_position.x + s_bulletSpeed * deltaSeconds),
          (float)(m_position.y + s_bulletSpeed * deltaSeconds)};
}

Vector2 Bullet::getColliderCenterByDelta(double delta) const {
  if(m_state == BulletStateFired) {
    float magnitude = m_speed * delta;
    Vector2 pos = { m_collider->position.x, m_collider->position.y + magnitude };

    // 목적지에 도착을 했으면 목적지 위치의 충돌체 중심 좌표를 반환
    if ((m_destination - pos).magnitude() <= magnitude) {
      // return (m_destination + m_size);
      // 목적지 도착 시 충돌 검사는 의미가 없음
      return {-1, -1};
    } else {
      return pos;
    }
  }
  return m_collider->position;
}

}  // namespace shmup
