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
  void speed(float speed);
  
  float speed() const;

  void state(BulletState state);

  BulletState state() const;

private:
  BulletState m_state;
  
  float m_speed;
};

}