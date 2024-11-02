//------------------------------------------------------------------------------
// File: Player.hpp
// Author: Chris Redwood
// Created: 2024-10-30
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include <SDL.h>

#include <list>
#include <memory>

#include "GameObject.hpp"
#include "TGA.hpp"

namespace shmup {

enum BulletState {
  BulletStateIdle,
  BulletStateFired,
  BulletStateHit,
  BulletStateDestroyed
};

struct Bullet : public GameObject {
  BulletState state;
  bool hasFlaggedCollided;
  float speed;
  void onCollided(const GameObject& target) override;
};

class Player : public GameObject {
 public:
  Player();

  ~Player();

  bool loadResource(SDL_Renderer* renderer);

  void updatePosition(float x, float y);

  void updateState(double delta);

  void move(int direction);

  inline std::unique_ptr<TGA>& planeTexture() { return m_planeTexture; }

  inline std::unique_ptr<TGA>& bulletTexture() { return m_bulletTexture; }

  void spawnBulets(unsigned count);

  void updateBulletPosition(double delta);

  inline const Bullet* bullets() { return m_bullets; }

  inline unsigned bulletCount() const { return m_bulletCount; }

  const std::vector<SDL_FPoint>& debugColliderPoints() {
    return m_debugColliderPoints;
  }

  void onCollided(const GameObject& target) override;

 private:
  std::unique_ptr<TGA> m_planeTexture;

  std::unique_ptr<TGA> m_bulletTexture;

  unsigned m_hp;

  float m_queuedMovePositionX = 0.0f;

  Bullet* m_bullets;

  unsigned m_bulletCount;

  double m_elapsedFireTime;

  std::vector<SDL_FPoint> m_debugColliderPoints;
};

}  // namespace shmup
