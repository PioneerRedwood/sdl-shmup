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

void drawStars(shmup::SDLRenderer& renderer,
               const shmup::TGA& tga, const shmup::Star* stars,
               unsigned starCount) {
  // renderer->enableBlending(SDL_BLENDMODE_BLEND); // TODO: only pixel
  // rendering requires enabling blending
  SDL_Texture* tex = const_cast<SDL_Texture*>(tga.sdlTexture());
  SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
  SDL_FRect rect;
  for (unsigned i = 0; i < starCount; ++i) {
    const shmup::Star& star = stars[i];
    if (star.isVisible()) {
      rect.w = star.size().x, rect.h = star.size().y;
      rect.x = star.position().x, rect.y = star.position().y;
      SDL_RenderCopyF(renderer.native(), tex, nullptr, &rect);
    }
  }
}

void drawPlayer(shmup::SDLRenderer& renderer,
                const shmup::Player& player) {
  renderer.enableBlending(SDL_BLENDMODE_BLEND);
  renderer.drawTGA(player.planeTexture(), player.position().x,
                    player.position().y);
}

void drawBullets(shmup::SDLRenderer& renderer,
                 const shmup::TGA& tex, const shmup::Bullet* bullets,
                 unsigned bulletCount) {
  SDL_Texture* texture = const_cast<SDL_Texture*>(tex.sdlTexture());
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  SDL_FRect rect;
  for (unsigned i = 0; i < bulletCount; ++i) {
    const shmup::Bullet& bullet = bullets[i];
    if (bullet.isVisible()) {
      rect.w = bullet.size().x, rect.h = bullet.size().y;
      rect.x = bullet.position().x, rect.y = bullet.position().y;
      SDL_RenderCopyF(renderer.native(), texture, nullptr, &rect);
    }
  }
}

void drawEnemies(shmup::SDLRenderer& renderer,
                 const shmup::TGA& tex, const shmup::Enemy* enemies,
                 unsigned enemyCount) {
  // TODO: Draw enemies
  SDL_Texture* texture = const_cast<SDL_Texture*>(tex.sdlTexture());
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  SDL_FRect rect;
  for (unsigned i = 0; i < enemyCount; ++i) {
    const shmup::Enemy& enemy = enemies[i];
    if (enemy.isVisible()) {
      rect.w = enemy.size().x, rect.h = enemy.size().y;
      rect.x = enemy.position().x, rect.y = enemy.position().y;
      SDL_RenderCopyF(renderer.native(), texture, nullptr, &rect);
    }
  }
}

void drawColliderLayers(shmup::SDLRenderer& renderer,
                        const shmup::Enemy* enemies, unsigned enemyCount,
                        const shmup::Vector2* playerColliderPoints) {
  // Enemy 충돌체 레이어 그리기
  for (unsigned i = 0; i < enemyCount; ++i) {
    const shmup::Enemy* enemy = &enemies[i];
    if (enemy->isVisible() == false) {
      continue;
    }
    SDL_SetRenderDrawColor(renderer.native(), 255, 255, 255, 255);
    SDL_RenderDrawPointsF(renderer.native(), (const SDL_FPoint*)enemy->debugColliderPoints(), 180);
  }

  // Player 충돌체 레이어 그리기
  SDL_SetRenderDrawColor(renderer.native(), 255, 255, 255, 255);
  SDL_RenderDrawPointsF(renderer.native(), (const SDL_FPoint*)playerColliderPoints, 180);

  // Bullet 충돌체 레이어 그리기
}

/// @brief 충돌 검사하면서 각 적나 총알의 상태가 변경되도록 플래그 설정
/// 프레임 스킵이 발생했을 때 처리되도록 수행
void performCollisionChecks(shmup::EnemyManager* enemyManager,
                  shmup::Player* player,
                  double delta) {
  using namespace shmup;

  // 쿼드 트리를 사용할 수 있을까?
  for (unsigned i = 0; i < enemyManager->enemyCount(); ++i) {
    // player <-> enemies
    Enemy* enemy = &enemyManager->enemies()[i];

    if (player->isVisible() && enemy->isVisible() && GameObject::isCollided(*player, *enemy)) {
      player->onCollided(*enemy);
      enemy->onCollided(*player);
    }

    // Enemies <-> bullet
    for (unsigned i = 0; i < player->bulletCount(); ++i) {
      Bullet* bullet = &player->bullets()[i];
      if (bullet->isVisible() && enemy->isVisible() && GameObject::isCollided(*bullet, *enemy)) {
        enemy->onCollided(*bullet);
        bullet->onCollided(*enemy);
      }
    }
  }
}

#define DEBUG true

int main(int argc, char** argv) {
  std::srand((unsigned)time(nullptr));

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

  shmup::StarManager* starManager = new shmup::StarManager();
  if (starManager->init(nativeRenderer, program->width(), program->height()) ==
      false) {
    return 1;
  }
  starManager->spawnStars(10);

  shmup::Player* player = new shmup::Player();
  if (player->loadResource(nativeRenderer) == false) {
    return 1;
  }
  // TODO: 플레이어 시작 지점 설정
  const SDL_Point startPoint = {
      (int)(program->width() / 2 -
            (player->planeTexture().header()->width / 2)),
      (int)(program->height() - player->planeTexture().header()->height)};
  player->updatePosition(startPoint.x, startPoint.y);
  program->bindKeyEvent([&player, &program](SDL_Event* event) {
    switch (event->key.keysym.sym) {
      case SDLK_RIGHT: {
        // std::cout << "KeyEvent move right >> \n";
        player->move(1);
        break;
      }
      case SDLK_LEFT: {
        // std::cout << "KeyEvent move left >> \n";
        player->move(-1);
        break;
      }
      default: {
        break;
      }
    }
  });

  shmup::EnemyManager* enemyManager = new shmup::EnemyManager();
  if (enemyManager->init(nativeRenderer, program->width(), program->height()) ==
      false) {
    program->quit();
  }

  // Main loop
  program->updateTime();
  while (program->neededQuit() == false) {
    // Update delta
    program->updateTime();

    performCollisionChecks(enemyManager, player, program->delta());

    // Handle input events
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
       program->handleEvent(&event);
    }

    // std::cout << "Delta: " << program->delta() << std::endl;

    // 각 상태 변화
    starManager->updateState(program->delta());
    player->updateState(program->delta());
    enemyManager->updateState(program->delta());

    // Rendering
    SDL_SetRenderDrawColor(nativeRenderer, 12, 10, 40, 255);
    renderer.clear();
    renderer.disableBlending();

    drawStars(renderer, starManager->tga(), starManager->stars(),
              starManager->starCount());
    drawPlayer(renderer, *player);
    drawBullets(renderer, player->bulletTexture(), player->bullets(),
                player->bulletCount());
    drawEnemies(renderer, enemyManager->enemyTexture(), enemyManager->enemies(),
                enemyManager->enemyCount());
    drawColliderLayers(renderer, enemyManager->enemies(),
                       enemyManager->enemyCount(),
                       player->debugColliderPoints());

    renderer.present();

    SDL_Delay(32);  // 16ms delayed
  }

  return 0;
}
