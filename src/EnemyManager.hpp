//------------------------------------------------------------------------------
// File: EnemyManager.hpp
// Author: Chris Redwood
// Created: 2024-10-31
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include <SDL.h>

#include <memory>

#include "GameObject.hpp"
#include "TGA.hpp"

namespace shmup {

struct Enemy : public GameObject {
  // TODO: add some stuff
};

class EnemyManager {
public:
  EnemyManager();

  ~EnemyManager();

  bool init(SDL_Renderer* renderer);

  void spawnEnemies(unsigned count);

  void updateEnemyPositions(double delta);

private:
  std::unique_ptr<TGA> m_texture;
};

}  // namespace shmup

