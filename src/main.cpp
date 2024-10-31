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

#include "EnemyManager.hpp"
#include "Player.hpp"
#include "SDLProgram.hpp"
#include "StarManager.hpp"
#include "TGA.hpp"

void drawStars(std::unique_ptr<shmup::SDLRenderer>& renderer,
               std::unique_ptr<shmup::TGA>& tga,
               const std::list<shmup::Star>& stars) {
  renderer->enableBlending(SDL_BLENDMODE_BLEND);
  SDL_Texture* tex = const_cast<SDL_Texture*>(tga->sdlTexture());
  SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
  SDL_FRect rect;
  for (auto& star : stars) {
    if (star.visible()) {
      rect.w = star.size().x, rect.h = star.size().y;
      rect.x = star.position().x, rect.y = star.position().y;
      SDL_RenderCopyF(renderer->native(), tex, nullptr, &rect);
    }
  }
}

void drawPlayer(std::unique_ptr<shmup::SDLRenderer>& renderer,
                std::unique_ptr<shmup::Player>& player) {
  renderer->enableBlending(SDL_BLENDMODE_BLEND);

  // TODO: Draw plane

  // TODO: Draw bullets
}

void drawEnemies(std::unique_ptr<shmup::SDLRenderer>& renderer,
                 std::unique_ptr<shmup::TGA>& tga,
                 const std::list<shmup::Enemy>& enemies) {
  // TODO: Draw enemies
}

int main(int argc, char** argv) {
  std::srand((unsigned)time(nullptr));

  // std::unique_ptr<SDLProgram> program = std::make_unique<SDLProgram>();
  auto* program = shmup::SDLProgram::instance();
  // TODO: width, height 값은 변경 가능
  if (program->init(640, 960) == false) {
    return 1;
  }

  auto& renderer = program->renderer();
  auto* nativeRenderer = program->nativeRenderer();

  std::unique_ptr<shmup::StarManager> starManager =
      std::make_unique<shmup::StarManager>();
  if (starManager->init(nativeRenderer, program->width(), program->height()) ==
      false) {
    program->quit();
    return 1;
  }
  starManager->spawnStars(140);

  std::unique_ptr<shmup::Player> player = std::make_unique<shmup::Player>();
  if (player->loadResource(nativeRenderer) == false) {
    program->quit();
    return 1;
  }
  // TODO: 플레이어 시작 지점이 맥북에서 제대로 나오지 않음: 수정 요망
  player->updatePosition(
      (int)program->width() / 2 - (player->plane()->header()->width / 2),
      program->height() - (150.0f + player->plane()->header()->height));
  program->bindKeyEvent([&player, &program](SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
      SDL_Keycode code = event->key.keysym.sym;
      if (code == SDLK_RIGHT) {
        std::cout << "Move right \n";
        player->move(program->delta());
      } else if (code == SDLK_LEFT) {
        std::cout << "Move left \n";
        player->move((-1) * program->delta());
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
      starManager->updatePositions(program->delta());
    }

    {  // Rendering
      SDL_SetRenderDrawColor(nativeRenderer, 0, 0, 0, 255);
      renderer->clear();
      renderer->disableBlending();

      // Draw stars
      drawStars(renderer, starManager->tga(), starManager->stars());

      // Draw Plane
      drawPlayer(renderer, player);

      // Render
      program->renderer()->present();
    }

    SDL_Delay(10);  // 16ms delayed
  }

  return 0;
}
