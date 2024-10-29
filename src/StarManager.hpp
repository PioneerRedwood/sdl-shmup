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
#include "Star.hpp"

namespace shmup {

class StarManager {
public:
  StarManager();

  ~StarManager();

  void spawnStars(unsigned count);

  void updatePositions(float delta);

  inline std::unique_ptr<TGA>& tga() {
    return m_tga;
  }

  inline const std::list<Star>& stars() {
    return m_stars;
  }

private:
  std::list<Star> m_stars;

  std::unique_ptr<TGA> m_tga;
};

}