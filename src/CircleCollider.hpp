//------------------------------------------------------------------------------
// File: CircleCollider.hpp
// Author: Chris Redwood
// Created: 2024-11-03
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include <SDL.h>

namespace shmup {

struct CircleCollider {
  SDL_FPoint position;
  float radius;
};

}  // namespace shmup