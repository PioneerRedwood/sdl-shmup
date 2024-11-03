//------------------------------------------------------------------------------
// File: EnemyManager.hpp
// Author: Chris Redwood
// Created: 2024-10-31
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include <SDL.h>

#include <list>
#include <memory>
#include <vector>

#include "GameObject.hpp"
#include "TGA.hpp"

namespace shmup {

enum EnemyState {
  EnemyStateIdle,
  EnemyStateHit,
  EnemyStateMove,
};

struct Enemy : public GameObject {
  // override
  void onCollided(const GameObject& target) override;
  void updateCollidePosition(SDL_FPoint pos);

  // properties
  std::vector<SDL_FPoint> debugColliderPoints;
  float speed;
  EnemyState state;
};

class EnemyManager {
public:
  EnemyManager();

  ~EnemyManager();

  bool init(SDL_Renderer* renderer, int width, int height);

  void spawnEnemies(unsigned count);

  void updateState(double delta);

  inline const Enemy* enemies() {
    return m_enemies;
  }

  inline unsigned enemyCount() const {
    return m_enemyCount;
  }

  inline std::unique_ptr<TGA>& enemyTexture() {
    return m_texture;
  }

private:
  void setEnemyRandomPos(Enemy* enemy);

private:
  std::unique_ptr<TGA> m_texture;

  Enemy* m_enemies = nullptr;

  unsigned m_enemyCount = 0;
};

}  // namespace shmup

