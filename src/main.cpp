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
#include "Blend.hpp"

#define DRAW_PIXELS_ONCE true
#define DRAW_EACH_PIXELS false
#define DRAW_COLLIDER false // for debugging

void drawStars(shmup::SDLRenderer& renderer,
               const shmup::TGA& tga, const shmup::Star* stars,
               unsigned starCount) {
#if DRAW_EACH_PIXELS
   renderer.enableBlending(SDL_BLENDMODE_BLEND);
   for (unsigned i = 0; i < starCount; ++i) {
     const shmup::Star* star = &stars[i];
     if (star->isVisible()) {
       renderer.drawTGA(tga, star->position().x, star->position().y);
     }
   }
#else
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
#endif
}

void drawPlayer(shmup::SDLRenderer& renderer,
                const shmup::Player& player) {
  renderer.enableBlending(SDL_BLENDMODE_BLEND);
  renderer.drawTGA(player.planeTexture(), player.position().x,
                    player.position().y);
}

void drawBullets(shmup::SDLRenderer& renderer,
                 const shmup::TGA& tga, const shmup::Bullet* bullets,
                 unsigned bulletCount) {
#if DRAW_EACH_PIXELS
  renderer.enableBlending(SDL_BLENDMODE_BLEND);
  for (unsigned i = 0; i < bulletCount; ++i) {
    const shmup::Bullet* bullet = &bullets[i];
    if (bullet->isVisible()) {
      renderer.drawTGA(tga, bullet->position().x, bullet->position().y);
    }
  }
#else
  SDL_Texture* texture = const_cast<SDL_Texture*>(tga.sdlTexture());
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
#endif
}

void drawEnemies(shmup::SDLRenderer& renderer,
                 const shmup::TGA& tga, const shmup::Enemy* enemies,
                 unsigned enemyCount) {
#if DRAW_EACH_PIXELS
  renderer.enableBlending(SDL_BLENDMODE_BLEND);
  for (unsigned i = 0; i < enemyCount; ++i) {
    const shmup::Enemy* enemy = &enemies[i];
    if (enemy->isVisible()) {
      renderer.drawTGA(tga, enemy->position().x, enemy->position().y);
    }
  }
#else
  SDL_Texture* texture = const_cast<SDL_Texture*>(tga.sdlTexture());
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
#endif
}

void drawColliderLayers(shmup::SDLRenderer& renderer,
                        const shmup::Enemy* enemies, unsigned enemyCount,
                        const shmup::Vector2* playerColliderPoints,
                        const shmup::Bullet* bullets, unsigned bulletCount) {
#if DRAW_COLLIDER
  SDL_SetRenderDrawColor(renderer.native(), 255, 255, 255, 255);

  // Enemy 충돌체 레이어 그리기
  for (unsigned i = 0; i < enemyCount; ++i) {
    const shmup::Enemy* enemy = &enemies[i];
    if (enemy == nullptr) {
      break;
    }

    if (enemy->isVisible() == false) {
      continue;
    }
    SDL_RenderDrawPointsF(renderer.native(), (const SDL_FPoint*)enemy->debugColliderPoints(), 180);
  }

  // Player 충돌체 레이어 그리기
  SDL_RenderDrawPointsF(renderer.native(), (const SDL_FPoint*)playerColliderPoints, 180);

  // Bullet 충돌체 레이어 그리기
  for (unsigned i = 0; i < bulletCount; ++i) {
    const shmup::Bullet* bullet = &bullets[i];
    if (bullet == nullptr) {
      break;
    }

    if (bullet->isVisible() == false) {
      continue;
    }
    SDL_RenderDrawPointsF(renderer.native(), (const SDL_FPoint*)bullet->debugPoints(), 180);
  }
#endif
}

/// @brief 충돌 검사하면서 각 적나 총알의 상태가 변경되도록 플래그 설정
/// 프레임 스킵이 발생했을 때 처리되도록 수행
/// - 공간분할을 통해 빠르게 할 수 있음; 예시로 쿼드 트리가 있음
constexpr float s_targetFrameTime = 1000.0f / 30; // 목표 프레임 0.3333..
void performCollisionChecks(shmup::EnemyManager* enemyManager,
                  shmup::Player* player,
                  double delta) {
  using namespace shmup;
  /*
    프레임 스킵이 발생하면 오브젝트는 스킵된 만큼의 위치로 이동하여 충돌 판정으로부터 벗어날 수 있다.
    프레임 스킵에 대한 적절한 처리로 모든 오브젝트의 충돌 판정을 수행한다.
    
    한 프레임 당 16ms 간격이 있다고 가정했을 때, 스킵된 프레임만큼 움직이는 오브젝트들의 예상 위치를 구할 수 있다. 
    예를 들어, 프레임 당 Y축으로 10만큼 이동하는 오브젝트는 50ms만큼의 프레임이 스킵된 경우 
    16ms, 32ms 그리고 48ms 시점의 위치를 각 계산할 수 있는 것이다. 

    이때 각 오브젝트의 목적지를 지나지는 않았는지 고려해야 한다. 
      - 에너미와 불릿 같은 경우에는 목적지를 지나면 곧바로 visible이 꺼지기 때문. 
    
    그러므로 목표한 프레임 간격(n)으로 설정되고 시간(d)이 지난 경우 (int)(d / n) + 1개 만큼 
    이전 프레임에서의 위치를 예상할 수 있을 것이다. 
     - 예상 프레임 타임 [ n * 1, n * 2 ..., n * (int)(d / n), d ]
     - 예상 위치 [ s * n * 1, s * n * 2 ..., s * n * (int)(d / n), s * d ]
  */
  bool hasFrameSkipped = (delta >= (s_targetFrameTime * 1.5f)); // 이 값은 변경될 수 있음
  //if(hasFrameSkipped) {
  //  // Frame has been skipped
  //  std::cout << "Frame skipped! delta: " << delta << ", target: " << s_targetFrameTime << std::endl;
  //} else {
  //  std::cout << "Delta: " << delta << std::endl;
  //}

  unsigned skippedFrameCount = (unsigned)(delta / s_targetFrameTime);

  for (unsigned i = 0; i < enemyManager->enemyCount(); ++i) {
    // player <-> enemies
    Enemy* enemy = &enemyManager->enemies()[i];
    if(enemy == nullptr) continue;

    if (player->isVisible() && enemy->isVisible()) {
      // 만약 프레임 스킵이 일어났다면 배열의 형태로 스킵된 프레임마다 오브젝트의 좌표를 구하고 비교
      bool isCollided = false;
      if(hasFrameSkipped) {
        for(unsigned j = 1; j < skippedFrameCount; ++j) {
          // 두 콜라이더의 정점
          Vector2 enemyPos = enemy->getColliderCenterByDelta(j * s_targetFrameTime);
          Vector2 playerPos = player->getColliderCenterByDelta(j * s_targetFrameTime);

          // 적 좌표가 -1.0f, -1.0f 한번이라도 나오면 루프 탈출
          if(enemyPos == Vector2(-1.0f, -1.0f)) {
            break;
          }

          float distance = Math::distance(enemyPos, playerPos);
          if(distance <= (enemy->collider()->radius + player->collider()->radius)) {
            isCollided = true;
            break;
          }
        }
      } else {
        isCollided = GameObject::isCollided(*player, *enemy);
      }

      if(isCollided) {
        //std::cout << "Collision! enemy <-> player \n";
        player->onCollided(*enemy);
        enemy->onCollided(*player);
      }
    }
    // Enemies <-> bullet
    for (unsigned j = 0; j < player->bulletCount(); ++j) {
      Bullet* bullet = &player->bullets()[j];
      if(bullet == nullptr) { continue; }

      if (bullet->isVisible() && enemy->isVisible()) {
        // 만약 프레임 스킵이 일어났다면 배열의 형태로 검사했을 프레임들의
        // 좌표를 일일이 구한다
        bool isCollided = false;
        if (hasFrameSkipped) {
          for (unsigned k = 1; k < skippedFrameCount; ++k) {
            // 두 콜라이더의 정점
            Vector2 enemyPos =
                enemy->getColliderCenterByDelta((double)k * s_targetFrameTime);
            Vector2 bulletPos =
                bullet->getColliderCenterByDelta((double)k * s_targetFrameTime);

            // 적 좌표가 -1.0f, -1.0f 한번이라도 나오면 루프 탈출
            if (enemyPos == Vector2(-1.0f, -1.0f) || bulletPos == Vector2(-1.0f, -1.0f)) {
              break;
            }

            float distance = Math::distance(enemyPos, bulletPos);
            if (distance <=
                (enemy->collider()->radius + bullet->collider()->radius)) {
              // printf(
              //     "Collision Y [cur: bullet %.1f enemy %.1f] [%.1f "
              //     "later: bullet %.1f enemy %.1f] \n",
              //     bullet->collider()->position.y, enemy->collider()->position.y,
              //     (double)k * s_targetFrameTime, bulletPos.y,
              //     enemyPos.y);
              isCollided = true;
              break;
            }
          }
        } else {
          isCollided = GameObject::isCollided(*enemy, *bullet);
        }

        if (isCollided) {
          // std::cout << "Collision! enemy <-> bullet \n";
          enemy->onCollided(*bullet);
          bullet->onCollided(*enemy);
        }
      }
    }
  }
}

int main(int argc, char** argv) {
  std::srand((unsigned)time(nullptr));

  shmup::SDLProgram* program = shmup::SDLProgram::instance();

  if (program->init(400, 0, 480, 640) == false) {
    return 1;
  }

  auto& renderer = program->renderer();
  auto* nativeRenderer = program->nativeRenderer();

  shmup::StarManager* starManager = new shmup::StarManager();
  if (starManager->init(nativeRenderer, program->width(), program->height(), 100) ==
      false) {
    return 1;
  }

  shmup::Player* player = new shmup::Player();
  if (player->loadResource(nativeRenderer) == false) {
    return 1;
  }

  const SDL_Point startPoint = {
      (int)(program->width() / 2 -
            (player->planeTexture().header()->width / 2)),
      (int)(program->height() - player->planeTexture().header()->height)};
  player->updatePosition(startPoint.x, startPoint.y);

  shmup::EnemyManager* enemyManager = new shmup::EnemyManager();
  if (enemyManager->init(nativeRenderer, program->width(), program->height()) ==
      false) {
    program->quit();
  }

  // Main loop
  program->updateTime();
  while (program->neededQuit() == false) {
    
    // 30fps같이 고정 목표 프레임이 있는 경우 아래와 같은 방식으로도 구현 가능
    // 여기서 delta 구하기
    // 시간이 목표한 0.3 보다 작으면 누적하고 업데이트 스킵 
    // 누적 + 현재 델타가 0.3보다 크면 0.3만큼으로 쪼개서 업데이트 
    //  예를 들어 1초인 경우 0.3 * 3 으로 3번 업데이트하고 0.1 누적 
    //update();

    {
      // Update delta
      program->updateTime();
      
      // Handle input events
      SDL_Event event;
      while (SDL_PollEvent(&event) != 0) {
        int move = 0;
        switch (event.type) {
        case SDL_QUIT: {
          program->quit();
          return 0;
        }
        case SDL_KEYDOWN: {
          switch (event.key.keysym.sym) {
          case SDLK_RIGHT: {
            move = 1;
            break;
          }
          case SDLK_LEFT: {
            move = -1;
            break;
          }
          default: {
            break;
          }
          }
          break;
        }
        default: {
          break;
        }
        }

        player->move(move);
      }

      // 각 상태 변화
      starManager->updateState(program->delta());
      player->updateState(program->delta());
      enemyManager->updateState(program->delta());

      // 충돌 검사
      performCollisionChecks(enemyManager, player, program->delta());
    }

#if TEST_PREMULTIPLIED_ALPHA
    // 비교 Alpha vs. Premultiplied Alpha 
    SDL_SetRenderDrawColor(nativeRenderer, 255, 0, 0, 1);
    Uint8 r = 0, g = 0, b = 0, a = 0;
    SDL_GetRenderDrawColor(nativeRenderer, &r, &g, &b, &a);
    //SDL_SetRenderDrawBlendMode(nativeRenderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawBlendMode(nativeRenderer, SDL_BLENDMODE_BLEND);
    renderer.clear();
    renderer.flush();
    drawPlayer(renderer, *player);
#elif DRAW_PIXELS_ONCE
    // 배경 그리기
    const shmup::RGBA spaceColor = { 12, 10, 40, 255 };
    renderer.clearColor(spaceColor);
    
    SDL_FRect rect;
    // 스타 그리기
    for (int i = 0; i < starManager->starCount(); ++i)
    {
      const shmup::Star *s = &starManager->stars()[i];
      if(s->isVisible() == false) {
        continue;
      }
      const shmup::RGBA *tgaPixels = starManager->tga().pixelData();
      rect.w = s->size().x, rect.h = s->size().y;
      rect.x = s->position().x, rect.y = s->position().y;

      renderer.renderPixels(tgaPixels, rect);
    }
    // 플레이어 그리기
    const shmup::RGBA* playerPixels = player->planeTexture().pixelData();
    rect.w = player->size().x, rect.h = player->size().y;
    rect.x = player->position().x, rect.y = player->position().y;
    renderer.renderPixels(playerPixels, rect);

    // 총알 그리기
    for(int i = 0; i < player->bulletCount(); ++i) {
      const shmup::Bullet* b = &player->bullets()[i];
      if(b->isVisible() == false) {
        continue;
      }
      const shmup::RGBA* tgaPixels = player->bulletTexture().pixelData();
      rect.w = b->size().x, rect.h = b->size().y;
      rect.x = b->position().x, rect.y = b->position().y;

      renderer.renderPixels(tgaPixels, rect);
    }

    // 적 그리기
    for(int i = 0; i < enemyManager->enemyCount(); ++i) {
      const shmup::Enemy* e = &enemyManager->enemies()[i];
      if(e->isVisible() == false) {
        continue;
      }
      const shmup::RGBA* tgaPixels = enemyManager->enemyTexture().pixelData();
      rect.w = e->size().x, rect.h = e->size().y;
      rect.x = e->position().x, rect.y = e->position().y;

      renderer.renderPixels(tgaPixels, rect);
    }
    
    SDL_RenderClear(nativeRenderer);
    SDL_UpdateTexture(renderer.m_frameTexture, nullptr, renderer.m_screenBuffer, 
                      program->width() * sizeof(shmup::RGBA));
    SDL_RenderCopy(nativeRenderer, renderer.m_frameTexture, NULL, NULL);
#else
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
                       player->debugColliderPoints(), player->bullets(),
                       player->bulletCount());
#endif
    renderer.present();

    //SDL_Delay(1);  // Almost no delayed
//    SDL_Delay(16);  // 16ms delayed
    //SDL_Delay(16 + rand() / ((RAND_MAX + 1u) / 64));  // 16 ~ 80ms random delayed
  }

  return 0;
}
