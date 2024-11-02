//------------------------------------------------------------------------------
// File: SDLProgram.hpp
// Author: Chris Redwood
// Created: 2024-10-21
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include <SDL.h>

#include <memory>
#include <mutex>

#include "SDLRenderer.hpp"

namespace shmup {

class SDLProgram {
 public:
  static SDLProgram* instance();

  ~SDLProgram();

  bool init(int x, int y, int width, int height);

  void quit();

  inline SDL_Window* window() {
    return m_window;
  }

  SDL_Renderer* nativeRenderer();

  inline std::unique_ptr<SDLRenderer>& renderer() {
    return m_renderer;
  }

  inline bool neededQuit() const {
    return m_neededQuit;
  }

  void handleEvent(SDL_Event* event);

  inline unsigned width() const { return m_width; }

  inline unsigned height() const { return m_height; }

  void updateTime();

  inline double delta() const { return m_delta; }

  void bindKeyEvent(std::function<void(SDL_Event*)>&& callback);

private:
  SDLProgram() = default;

  static SDLProgram* s_instance;

  SDL_Window* m_window = nullptr;

  std::unique_ptr<SDLRenderer> m_renderer;

  bool m_neededQuit = false;

  unsigned m_width = 0;

  unsigned m_height = 0;

  uint64_t m_currentTime = 0;

  uint64_t m_lastTime = 0;

  double m_delta = 0;

  std::function<void(SDL_Event*)> m_keyEventCallback;
};

}  // namespace shmup
