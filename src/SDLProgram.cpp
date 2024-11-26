//------------------------------------------------------------------------------
// File: SDLProgram.cpp
// Author: Chris Redwood
// Created: 2024-10-21
// License: MIT License
//------------------------------------------------------------------------------

#include "SDLProgram.hpp"

#include <iostream>

namespace shmup {

SDLProgram* SDLProgram::s_instance = nullptr;

SDLProgram* SDLProgram::instance() {
  if (s_instance == nullptr) {
    s_instance = new SDLProgram();
  }
  return s_instance;
}

SDLProgram::~SDLProgram() { quit(); }

bool SDLProgram::init(int x, int y, int width, int height) {
  m_width = width;
  m_height = height;

  if (SDL_Init(SDL_INIT_EVENTS) < 0) {
    std::cout << "SDL_Init failed error: " << SDL_GetError() << std::endl;
    return false;
  }

  m_window = SDL_CreateWindow("SDL-Examples", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (m_window == nullptr) {
    std::cout << "SDL_CreateWindow failed error: " << SDL_GetError() << std::endl;
    return false;
  } else {
    int w, h;
    SDL_GetWindowSize(m_window, &w, &h);
    std::cout << "SDL_GetWindowSize w: " << w << "h: " << h << "\n";
  }

  m_renderer = new SDLRenderer();
  if(m_renderer->init(m_window, m_width, m_height) == false) {
    return false;
  }

  return true;
}

void SDLProgram::quit() {
  m_neededQuit = true;

  delete m_renderer;

  SDL_DestroyWindow(m_window);

  SDL_Quit();
}

SDL_Window* SDLProgram::window() {
  return m_window;
}

SDL_Renderer* SDLProgram::nativeRenderer() { return m_renderer->native(); }

SDLRenderer& SDLProgram::renderer() {
  return *m_renderer;
}

bool SDLProgram::neededQuit() const {
  return m_neededQuit;
}

unsigned SDLProgram::width() const {
  return m_width;
}

unsigned SDLProgram::height() const {
  return m_height;
}

void SDLProgram::updateTime() {
  m_lastTime = m_currentTime;
  m_currentTime = SDL_GetPerformanceCounter();
  m_delta = (double)((m_currentTime - m_lastTime) * 1000 /
                     (double)SDL_GetPerformanceFrequency());
  std::cout << "FPS: " << 1000 / m_delta << std::endl;
}

double SDLProgram::delta() const {
  return m_delta;
}

}  // namespace shmup
