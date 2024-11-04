//------------------------------------------------------------------------------
// File: Player.hpp
// Author: Chris Redwood
// Created: 2024-10-30
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include <SDL.h>

#include "GameObject.hpp"
#include "TGA.hpp"
#include "Bullet.hpp"

namespace shmup {

class Player : public GameObject {
 public:
  Player();

  ~Player();

  bool loadResource(SDL_Renderer* renderer);

  void updatePosition(float x, float y);

  void updateState(double delta);

  void move(int direction);

  const TGA& planeTexture() const { return *m_planeTexture; }

  const TGA& bulletTexture() const { return *m_bulletTexture; }

  void updateBulletPosition(double delta);

  const Bullet* bullets() { return m_bullets; }

  unsigned bulletCount() const { return m_bulletCount; }

  const SDL_FPoint* debugColliderPoints() {
    return m_debugColliderPoints;
  }

  void onCollided(const GameObject& target) override;

 private:
  TGA* m_planeTexture = nullptr;

  TGA* m_bulletTexture = nullptr;

  float m_queuedMovePositionX = 0.0f;

  Bullet* m_bullets = nullptr;

  unsigned m_bulletCount = 0;

  double m_elapsedFireTime = 0.0f;

  SDL_FPoint* m_debugColliderPoints = nullptr;
};

}  // namespace shmup
