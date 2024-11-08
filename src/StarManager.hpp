
//------------------------------------------------------------------------------
// File: StarManager.hpp
// Author: Chris Redwood
// Created: 2024-10-29
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include <SDL.h>

#include <RGBA.hpp>

#include "GameObject.hpp"
#include "TGA.hpp"

namespace shmup {

struct Star : public GameObject {
  float speed;

  void onCollided(const GameObject&) override {};
};

class StarManager {
 public:
  StarManager();

  ~StarManager();

  bool init(SDL_Renderer* renderer, int width, int height, unsigned starCount);

  void updateState(float delta);

  const TGA& tga();

  const Star* stars();

  unsigned starCount() const;

 private:
  void setStarRandomPos(Star* star);

 private:
  TGA* m_tga = nullptr;

  Star* m_stars = nullptr;

  unsigned m_starCount = 0;

  double m_lastStarSpawnTime = 0.0f;

  double m_starSpawnDelay = 0.0f;
};

}  // namespace shmup
