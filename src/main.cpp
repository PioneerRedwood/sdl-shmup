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
#include "Player.hpp"

int main(int argc, char** argv) {
  using namespace shmup;
  std::srand((unsigned)time(nullptr));

  // std::unique_ptr<SDLProgram> program = std::make_unique<SDLProgram>();
  auto* program = SDLProgram::instance();
  // TODO: width, height 값은 변경 가능
  if (program->init(640, 960) == false) {
    return 1;
  }

  auto& renderer = program->renderer();
  auto* nativeRenderer = program->nativeRenderer();

  StarManager starManager;
  starManager.spawnStars(140);

  Player player;
  if(player.loadResource(nativeRenderer) == false) {
    program->quit();
    return 1;
  }
  player.updatePosition(
    (int)program->width() / 2 - (player.plane()->header()->width / 2), 
    program->height() - (150.0f + player.plane()->header()->height));
  program->bindKeyEvent([&player, &program](SDL_Event* event) {
    if(event->type == SDL_KEYDOWN) {
        SDL_Keycode code = event->key.keysym.sym;
        if (code == SDLK_RIGHT) {
          std::cout << "Move right \n";
          player.move(program->delta());
        } else if (code == SDLK_LEFT) {
          std::cout << "Move left \n";
          player.move((-1) * program->delta());
        }
    }
  });

  program->updateTime();
  // Main loop
  while (program->neededQuit() == false) {
    {  
        // Check collision
        // TODO: 모든 GameObject를 상속받는 인스턴스끼리 비교
    }

    {  
        // Update delta
        program->updateTime();
    }

    {  // Handle input events
      SDL_Event event;
      while (SDL_PollEvent(&event) != 0) {
        program->handleEvent(&event);
      }
    }

    {  // Update spawnable stuff (Change states)
      starManager.updatePositions(program->delta());
    }

    {  // Rendering
      SDL_SetRenderDrawColor(nativeRenderer, 0, 0, 0, 255);
      renderer->clear();
      renderer->disableBlending();

      // Draw stars
      renderer->enableBlending(SDL_BLENDMODE_BLEND);
      renderer->drawStars(starManager.tga(), starManager.stars());

      // Draw Plane
      renderer->enableBlending(SDL_BLENDMODE_BLEND);
      renderer->drawTGA(player.plane(), player.position().x, player.position().y);
      renderer->flush();

      // Render
      program->renderer()->present();
    }

    SDL_Delay(10);  // 16ms delayed
  }

  return 0;
}

