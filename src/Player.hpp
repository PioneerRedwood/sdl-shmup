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

  Bullet* bullets() { return m_bullets; }

  unsigned bulletCount() const { return m_bulletCount; }

  const Vector2* debugColliderPoints() {
    return m_debugColliderPoints;
  }

  void onCollided(const GameObject& target) override;

  /// @brief 만약 움직이고 있다면 주어진 시간의 예상되는 콜라이더 중심 좌표 반환.
  /// 만약 움직이도록 설정되지 않았거나 목적지에 위치해 있다면 현재 위치 반환.
  Vector2 getColliderCenterByDelta(double delta) const;

 private:
  TGA* m_planeTexture = nullptr;

  TGA* m_bulletTexture = nullptr;

  bool m_isFlaggedToMove = false;

  Vector2 m_destPos;

  Bullet* m_bullets = nullptr;

  unsigned m_bulletCount = 0;

  double m_elapsedFireTime = 0.0f;

  Vector2* m_debugColliderPoints = nullptr;
};

}  // namespace shmup
