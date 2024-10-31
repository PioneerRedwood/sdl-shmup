//------------------------------------------------------------------------------
// File: EnemyManager.cpp
// Author: Chris Redwood
// Created: 2024-10-31
// License: MIT License
//------------------------------------------------------------------------------

#include "EnemyManager.hpp"

namespace shmup {

#if _WIN32
constexpr auto s_enemyFilepath = "";
#else
constexpr auto s_enemyFilepath = "";
#endif

EnemyManager::EnemyManager() {}

EnemyManager::~EnemyManager() {}

bool EnemyManager::init(SDL_Renderer* renderer) {
  // TODO: Load texture

  // TODO: Create texture

  return true;
}

void EnemyManager::spawnEnemies(unsigned count) {
  // TODO: Spawn enemies array

}

void EnemyManager::updateEnemyPositions(double delta) {
  // TODO: Update positions of enemies
}


}