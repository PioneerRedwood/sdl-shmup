
//------------------------------------------------------------------------------
// File: SDLRenderer.hpp
// Author: Chris Redwood
// Created: 2024-10-21
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include <SDL.h>

#include <list>
#include <memory>

#include "Player.hpp"
#include "RGBA.hpp"
#include "StarManager.hpp"
#include "TGA.hpp"

namespace shmup {

class SDLRenderer {
 public:
  SDLRenderer();
  ~SDLRenderer();

  SDLRenderer(const SDLRenderer&) = delete;
  SDLRenderer& operator=(const SDLRenderer&) = delete;

  SDLRenderer(SDLRenderer&&) = delete;
  SDLRenderer& operator=(SDLRenderer&&) = delete;

  bool init(SDL_Window* window);

  SDL_Renderer* native();

  void clear();

  void drawTGA(std::unique_ptr<TGA>& tga, int x, int y);

  void drawStars(std::unique_ptr<TGA>& tga, const std::list<Star>& stars);

  void drawPlayer(std::unique_ptr<Player>& player);

  void enableBlending(SDL_BlendMode blendMode);

  void disableBlending();

  void present();

  void flush();

 private:
  SDL_Renderer* m_renderer = nullptr;

  SDL_BlendMode m_currentBlendMode = SDL_BLENDMODE_NONE;
};

}  // namespace shmup
