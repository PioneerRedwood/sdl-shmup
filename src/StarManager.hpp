
//------------------------------------------------------------------------------
// File: StarManager.hpp
// Author: Chris Redwood
// Created: 2024-10-29
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include <SDL.h>
#include <RGBA.hpp>
#include <list>
#include "TGA.hpp"
#include "GameObject.hpp"

namespace shmup {

struct Star : public GameObject {
  float speed;
};

class StarManager {
public:
  StarManager();

  ~StarManager();

  bool init(SDL_Renderer* renderer, int width, int height);

  void spawnStars(unsigned count);

  void updateState(float delta);

  inline std::unique_ptr<TGA>& tga() {
    return m_tga;
  }

  inline const Star* stars() {
    return m_stars;
  }

  inline unsigned starCount() const {
    return m_starCount;
  }

private:
  void setStarRandomPos(Star* star);

private:
  std::unique_ptr<TGA> m_tga;

  Star* m_stars = nullptr;

  unsigned m_starCount = 0;
};

}

