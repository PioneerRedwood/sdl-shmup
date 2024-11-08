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

  m_window = SDL_CreateWindow("SDL-Examples", x, y, width, height,
                              SDL_WINDOW_SHOWN);
  if (m_window == nullptr) {
    std::cout << "SDL_CreateWindow failed error: " << SDL_GetError() << std::endl;
    return false;
  }

  m_renderer = new SDLRenderer();
  if(m_renderer->init(m_window) == false) {
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
}

double SDLProgram::delta() const {
  return m_delta;
}

}  // namespace shmup
