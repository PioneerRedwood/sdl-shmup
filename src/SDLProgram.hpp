//------------------------------------------------------------------------------
// File: SDLProgram.hpp
// Author: Chris Redwood
// Created: 2024-10-21
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include <SDL.h>

#include <functional>

#include "SDLRenderer.hpp"

namespace shmup {

class SDLProgram {
 public:
  static SDLProgram* instance();

  ~SDLProgram();

  bool init(int x, int y, int width, int height);

  void quit();

  SDL_Window* window();

  SDL_Renderer* nativeRenderer();

  SDLRenderer& renderer();

  bool neededQuit() const;

  void handleEvent(SDL_Event* event);

  unsigned width() const;

  unsigned height() const;

  void updateTime();

  double delta() const;

  void bindKeyEvent(std::function<void(SDL_Event*)>&& callback);

private:
  SDLProgram() = default;

  static SDLProgram* s_instance;

  SDL_Window* m_window = nullptr;

  SDLRenderer* m_renderer = nullptr;

  bool m_neededQuit = false;

  unsigned m_width = 0;

  unsigned m_height = 0;

  uint64_t m_currentTime = 0;

  uint64_t m_lastTime = 0;

  double m_delta = 0;

  // TODO: C style function으로 변경할 것
  std::function<void(SDL_Event*)> m_keyEventCallback;
};

}  // namespace shmup
