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
#include "Enemy.hpp"

namespace shmup {

class EnemyManager {
public:
  EnemyManager();

  ~EnemyManager();

  bool init(SDL_Renderer* renderer, int width, int height);

  void spawnEnemies(unsigned count);

  void updateState(double delta);

  const Enemy* enemies() {
    return m_enemies;
  }

  unsigned enemyCount() const {
    return m_enemyCount;
  }

  const TGA& enemyTexture() {
    return *m_texture;
  }

private:
  void setEnemyRandomPos(Enemy* enemy);

private:
  TGA* m_texture;

  Enemy* m_enemies = nullptr;

  unsigned m_enemyCount = 0;
};

}  // namespace shmup

