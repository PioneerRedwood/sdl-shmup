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
#include "Math.hpp"
#include "Player.hpp"
#include "SDLProgram.hpp"
#include "StarManager.hpp"
#include "TGA.hpp"

void drawStars(std::unique_ptr<shmup::SDLRenderer>& renderer,
               std::unique_ptr<shmup::TGA>& tga, const shmup::Star* stars,
               unsigned starCount) {
  // renderer->enableBlending(SDL_BLENDMODE_BLEND); // TODO: only pixel
  // rendering requires enabling blending
  SDL_Texture* tex = const_cast<SDL_Texture*>(tga->sdlTexture());
  SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
  SDL_FRect rect;
  for (unsigned i = 0; i < starCount; ++i) {
    const shmup::Star& star = stars[i];
    if (star.visible()) {
      rect.w = star.size().x, rect.h = star.size().y;
      rect.x = star.position().x, rect.y = star.position().y;
      SDL_RenderCopyF(renderer->native(), tex, nullptr, &rect);
    }
  }
}

std::vector<std::vector<SDL_FPoint>> s_colliderPointsList;

void drawPlayer(std::unique_ptr<shmup::SDLRenderer>& renderer,
                std::unique_ptr<shmup::Player>& player) {
  renderer->enableBlending(SDL_BLENDMODE_BLEND);

  // TODO: Draw plane
  renderer->drawTGA(player->planeTexture(), player->position().x,
                    player->position().y);
}

void drawBullets(std::unique_ptr<shmup::SDLRenderer>& renderer,
                 std::unique_ptr<shmup::TGA>& tex, const shmup::Bullet* bullets,
                 unsigned bulletCount) {
  SDL_Texture* texture = const_cast<SDL_Texture*>(tex->sdlTexture());
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  SDL_FRect rect;
  for (unsigned i = 0; i < bulletCount; ++i) {
    const shmup::Bullet& bullet = bullets[i];
    if (bullet.visible()) {
      rect.w = bullet.size().x, rect.h = bullet.size().y;
      rect.x = bullet.position().x, rect.y = bullet.position().y;
      SDL_RenderCopyF(renderer->native(), texture, nullptr, &rect);
    }
  }
}

void drawEnemies(std::unique_ptr<shmup::SDLRenderer>& renderer,
                 std::unique_ptr<shmup::TGA>& tex, const shmup::Enemy* enemies,
                 unsigned enemyCount) {
  // TODO: Draw enemies
  SDL_Texture* texture = const_cast<SDL_Texture*>(tex->sdlTexture());
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  SDL_FRect rect;
  for (unsigned i = 0; i < enemyCount; ++i) {
    const shmup::Enemy& enemy = enemies[i];
    if (enemy.visible()) {
      rect.w = enemy.size().x, rect.h = enemy.size().y;
      rect.x = enemy.position().x, rect.y = enemy.position().y;
      SDL_RenderCopyF(renderer->native(), texture, nullptr, &rect);
    }
  }
}

void drawColliderLayers(std::unique_ptr<shmup::SDLRenderer>& renderer,
                        const shmup::Enemy* enemies, unsigned enemyCount,
                        const std::vector<SDL_FPoint>& playerColliderPoints) {
  // Enemy 충돌체 레이어 그리기
  for (unsigned i = 0; i < enemyCount; ++i) {
    const shmup::Enemy* enemy = &enemies[i];
    if (enemy->visible() == false || enemy->debugColliderPoints.empty()) {
      continue;
    }
    SDL_SetRenderDrawColor(renderer->native(), 255, 255, 255, 255);
    SDL_RenderDrawPointsF(renderer->native(), enemy->debugColliderPoints.data(),
                          (int)enemy->debugColliderPoints.size());
  }

  // Player 충돌체 레이어 그리기
  SDL_SetRenderDrawColor(renderer->native(), 255, 255, 255, 255);
  SDL_RenderDrawPointsF(renderer->native(), playerColliderPoints.data(),
                        (int)playerColliderPoints.size());

  // Bullet 충돌체 레이어 그리기
}

int main(int argc, char** argv) {
  std::srand((unsigned)time(nullptr));

  // std::unique_ptr<SDLProgram> program = std::make_unique<SDLProgram>();
  auto* program = shmup::SDLProgram::instance();

  // TODO: width, height 값은 변경 가능 (640, 960 정도로 생각중)
#if _WIN32
  if (program->init(400, 0, 640, 960) == false) {
    return 1;
  }
#else
  if (program->init(400, 0, 480, 640) == false) {
    return 1;
  }
#endif

  auto& renderer = program->renderer();
  auto* nativeRenderer = program->nativeRenderer();

  std::unique_ptr<shmup::StarManager> starManager =
      std::make_unique<shmup::StarManager>();
  if (starManager->init(nativeRenderer, program->width(), program->height()) ==
      false) {
    return 1;
  }
  starManager->spawnStars(10);

  std::unique_ptr<shmup::Player> player = std::make_unique<shmup::Player>();
  if (player->loadResource(nativeRenderer) == false) {
    return 1;
  }
  // TODO: 플레이어 시작 지점 설정
  const SDL_Point startPoint = {
      (int)(program->width() / 2 -
            (player->planeTexture()->header()->width / 2)),
      (int)(program->height() - player->planeTexture()->header()->height)};
  player->updatePosition(startPoint.x, startPoint.y);
  program->bindKeyEvent([&player, &program](SDL_Event* event) {
    switch (event->key.keysym.sym) {
      case SDLK_RIGHT: {
        std::cout << "KeyEvent move right >> \n";
        player->move(1);
        break;
      }
      case SDLK_LEFT: {
        std::cout << "KeyEvent move left >> \n";
        player->move(-1);
        break;
      }
      default: {
        break;
      }
    }
  });

  std::unique_ptr<shmup::EnemyManager> enemyManager =
      std::make_unique<shmup::EnemyManager>();
  if (enemyManager->init(nativeRenderer, program->width(), program->height()) ==
      false) {
    program->quit();
  }
  enemyManager->spawnEnemies(10);

  // Main loop
  while (program->neededQuit() == false) {
#if DEBUG
    shmup::Bullet* b;
    shmup::Enemy* e;
#endif
    for (unsigned i = 0; i < enemyManager->enemyCount(); ++i) {
      // player <-> enemies
      shmup::GameObject* enemy =
          (shmup::GameObject*)&enemyManager->enemies()[i];
      if (shmup::GameObject::isCollided((const shmup::GameObject&)*player.get(),
                                        *enemy)) {
        player->onCollided(*enemy);
        enemy->onCollided(*player.get());
      }

      // Enemies <-> bullet
      for (unsigned i = 0; i < player->bulletCount(); ++i) {
        shmup::GameObject* bullet = (shmup::GameObject*)&player->bullets()[i];
        if (shmup::GameObject::isCollided(*bullet, *enemy)) {
          enemy->onCollided(*bullet);
          bullet->onCollided(*enemy);
        }

#if DEBUG
        e = (shmup::Enemy*)enemy;
        b = (shmup::Bullet*)bullet;
#endif
      }
    }

    // Update delta
    program->updateTime();

    // Handle input events
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
       program->handleEvent(&event);
    }

    std::cout << "Delta: " << program->delta() << std::endl;

    // Update spawnable stuff (Change states)
    starManager->updateState(program->delta());
    player->updateState(program->delta());
    enemyManager->updateState(program->delta());

    // Rendering
    SDL_SetRenderDrawColor(nativeRenderer, 12, 10, 40, 255);
    renderer->clear();
    renderer->disableBlending();

#if DEBUG
    SDL_SetRenderDrawColor(nativeRenderer, 255, 0, 0, 255);
    SDL_RenderDrawLineF(nativeRenderer, b->getColliderCenterPosition().x,
                        b->getColliderCenterPosition().y,
                        e->getColliderCenterPosition().x,
                        e->getColliderCenterPosition().y);
    SDL_RenderFlush(nativeRenderer);
#endif

    drawStars(renderer, starManager->tga(), starManager->stars(),
              starManager->starCount());
    drawPlayer(renderer, player);
    drawBullets(renderer, player->bulletTexture(), player->bullets(),
                player->bulletCount());
    drawEnemies(renderer, enemyManager->enemyTexture(), enemyManager->enemies(),
                enemyManager->enemyCount());
    drawColliderLayers(renderer, enemyManager->enemies(),
                       enemyManager->enemyCount(),
                       player->debugColliderPoints());

    program->renderer()->present();

    SDL_Delay(32);  // 16ms delayed
  }

  return 0;
}
