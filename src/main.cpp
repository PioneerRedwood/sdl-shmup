//------------------------------------------------------------------------------
// File: main.cpp
// Author: Chris Redwood
// Created: 2024-10-21
// License: MIT License
//------------------------------------------------------------------------------

#include <SDL.h>

#include <cstdlib>
#include <iostream>
#include <memory>

#include "SDLProgram.hpp"
#include "StarManager.hpp"
#include "TGA.hpp"

#if _WIN32
constexpr auto s_planeFilepath = "../resources/plane.tga";
#else
constexpr auto s_planeFilepath = "../../resources/plane.tga";
#endif

int main(int argc, char** argv) {
  using namespace shmup;
  std::srand((unsigned)time(nullptr));

  // std::unique_ptr<SDLProgram> program = std::make_unique<SDLProgram>();
  auto* program = SDLProgram::instance();
  // TODO: width, height 값은 변경 가능
  if (program->init(640, 960) == false) {
    return 1;
  }

  StarManager StarManager;
  StarManager.spawnStars(140);

  // 리소스 로드
  std::unique_ptr<TGA> plane = std::make_unique<TGA>();
  if (plane->readFromFile(s_planeFilepath) == false) {
    std::cout << "File read failed " << s_planeFilepath << std::endl;
    return 1;
  }
  auto& renderer = program->renderer();
  auto* nativeRenderer = program->nativeRenderer();

  program->updateTime();
  // Main loop
  while (program->neededQuit() == false) {
    program->updateTime();
    {  // Check collision
    }

    {  // Update delta
    }

    {  // Handle input events
      SDL_Event event;
      while (SDL_PollEvent(&event) != 0) {
        program->handleEvent(&event);
      }
    }

    {  // Update spawnable stuff (Change states)
      StarManager.updatePositions(program->delta());
    }

    {  // Rendering
      SDL_SetRenderDrawColor(nativeRenderer, 0, 0, 0, 255);
      renderer->clear();
      renderer->disableBlending();

      // Draw stars
      renderer->enableBlending(SDL_BLENDMODE_BLEND);
      renderer->drawStars(StarManager.tga(), StarManager.stars());

      // Draw Plane
      renderer->enableBlending(SDL_BLENDMODE_BLEND);
      renderer->drawTGA(plane, 0, 0);
      renderer->flush();

      // Render
      program->renderer()->present();
    }

    SDL_Delay(10);  // 16ms delayed
  }

  return 0;
}
