//------------------------------------------------------------------------------
// File: Bullet.hpp
// Author: Chris Redwood
// Created: 2024-11-04
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include <SDL.h>

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

public:
  void speed(float speed) { m_speed = speed; }
  
  float speed() const { return m_speed; }

  void state(BulletState state) { m_state = state; }

  BulletState state() const { return m_state; }

private:
  BulletState m_state;
  
  float m_speed;
};

}