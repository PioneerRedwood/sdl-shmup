//------------------------------------------------------------------------------
// File: Bullet.cpp
// Author: Chris Redwood
// Created: 2024-11-04
// License: MIT License
//------------------------------------------------------------------------------

#include "Bullet.hpp"

namespace shmup {

float s_bulletSpeed = 300.0f; // 300.0f 기본값

Bullet::Bullet() : GameObject() {
  // 기본값 설정
  m_size = { 10.0f, 10.0f };
  m_position = { 0.0f, 0.0f };
  setCollider(0.0f, 0.0f, 3.0f);
  m_visible = false;
  m_tag = GameObjectTagBullet;
  m_speed = s_bulletSpeed;
  m_state = BulletStateIdle;
}

Bullet::~Bullet() {}

void Bullet::speed(float speed) { m_speed = speed; }

float Bullet::speed() const { return m_speed; }

void Bullet::state(BulletState state) { m_state = state; }

BulletState Bullet::state() const { return m_state; }

void Bullet::onCollided(const GameObject& target) {
  if(target.tag() == GameObjectTagEnemy) {
    m_state = BulletStateIdle;
    // TODO: 맞았을 때 처리
    m_visible = false;
  }
}


}