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

bool SDLProgram::init(int width, int height) {
  m_width = width;
  m_height = height;

  // TODO: 프로그램 초기화
  if (SDL_Init(SDL_INIT_EVENTS) < 0) {
    // TODO: 초기화 시 문제 해결
    SDL_assert(false);
    return false;
  }

  m_window = SDL_CreateWindow("SDL-Examples", 400, 0, width, height,
                              SDL_WINDOW_RESIZABLE);
  if (m_window == nullptr) {
    SDL_assert(false);
    return false;
  }

  m_renderer = std::make_unique<SDLRenderer>(m_window);

  return true;
}

void SDLProgram::quit() {
  // TODO: 프로그램 종료
  // 렌더러를 먼저 삭제
  m_renderer.release();

  SDL_DestroyWindow(m_window);

  SDL_Quit();
}

SDL_Window* SDLProgram::window() { return m_window; }

SDL_Renderer* SDLProgram::nativeRenderer() { return m_renderer->native(); }

std::unique_ptr<SDLRenderer>& SDLProgram::renderer() { return m_renderer; }

bool SDLProgram::neededQuit() const { return m_neededQuit; }

void SDLProgram::handleEvent(SDL_Event* event) {
  switch (event->type) {
    case SDL_QUIT: {
      m_neededQuit = true;
      break;
    }
    case SDL_KEYDOWN: {
      m_keyEventCallback(event);
      break;
    }
    default: {
      break;
    }
  }
}

void SDLProgram::updateTime() {
  m_lastTime = m_currentTime;
  m_currentTime = SDL_GetPerformanceCounter();
  m_delta = (double)((m_currentTime - m_lastTime) * 1000 /
                     (double)SDL_GetPerformanceFrequency());
}

void SDLProgram::bindKeyEvent(std::function<void(SDL_Event*)>&& callback) {
    m_keyEventCallback = std::move(callback);
}

}  // namespace shmup
