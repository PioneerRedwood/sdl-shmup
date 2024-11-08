//------------------------------------------------------------------------------
// File: Bullet.hpp
// Author: Chris Redwood
// Created: 2024-11-04
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include "GameObject.hpp"

namespace shmup {

enum BulletState {
  BulletStateIdle,
  BulletStateFired,
};

class Bullet : public GameObject {
public:
  Bullet();
  
  ~Bullet();

  void onCollided(const GameObject& target) override;

  Vector2 nextPos(double delta) const;

  /// @brief 만약 움직이고 있다면 주어진 시간의 예상되는 콜라이더 중심 좌표 반환.
  /// 만약 움직이도록 설정되지 않았거나 목적지에 위치해 있다면 현재 위치 반환. 
  Vector2 getColliderCenterByDelta(double delta) const;

public:
  void speed(float speed);
  
  float speed() const;

  void state(BulletState state);

  BulletState state() const;

  Vector2* debugPoints() const;

  void destination(Vector2 pos) { m_destination = pos; }
  
  Vector2 destination() const {  return m_destination; }

private:
  BulletState m_state = BulletStateIdle;
  
  float m_speed;

  Vector2* m_debugPoints = nullptr;

  Vector2 m_destination;
};

}