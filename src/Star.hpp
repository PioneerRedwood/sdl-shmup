//------------------------------------------------------------------------------
// File: Star.hpp
// Author: Chris Redwood
// Created: 2024-10-29
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include <SDL.h>

namespace shmup {

struct Star {
  SDL_FRect rect;
  bool visible;
  float speed;
};

}