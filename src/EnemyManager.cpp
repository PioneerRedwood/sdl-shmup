//------------------------------------------------------------------------------
// File: EnemyManager.cpp
// Author: Chris Redwood
// Created: 2024-10-31
// License: MIT License
//------------------------------------------------------------------------------

#include "EnemyManager.hpp"

#include "Math.hpp"
namespace shmup {

#if _WIN32
constexpr auto s_enemyFilepath = "../resources/enemy.tga";
#else
constexpr auto s_enemyFilepath = "../../resources/enemy.tga";
#endif

unsigned s_enemyMaxXPos = 0;
unsigned s_enemyMaxYPos = 0;
float s_enemySpeed = 30.0f;
unsigned s_enemyDebugCircleColliderSteps = 180;
float s_enemyCircleColliderRadius = 3.0f;

EnemyManager::EnemyManager() {}

EnemyManager::~EnemyManager() {
  delete[] m_enemies;
  m_enemies = nullptr;
}

bool EnemyManager::init(SDL_Renderer* renderer, int width, int height) {
  m_texture = new TGA();
  if (m_texture->readFromFile(s_enemyFilepath) == false) {
    return false;
  }

  if (m_texture->createTexture(renderer) == false) {
    return false;
  }

  s_enemyCircleColliderRadius = m_texture->header()->width / 2;

  s_enemyMaxXPos = width - m_texture->header()->width;
  s_enemyMaxYPos = height - m_texture->header()->height * 2; // 밑에 완전히 사라질 정도

  return true;
}

void EnemyManager::setEnemyRandomPos(Enemy* enemy) {
  SDL_assert(m_texture != nullptr);

  SDL_FPoint value = {(float)m_texture->header()->width,
                      (float)m_texture->header()->height};
  enemy->size(value);

  value = {(float)(rand() /
                   ((RAND_MAX + 1u) / s_enemyMaxXPos)),  // 0 ~ s_enemyMaxXPos
           0.0f};
  enemy->position(value);
  
  enemy->speed((float)(10.0f + rand() / ((RAND_MAX + 1u) / 25)));

  // 충돌체 위치 업데이트
  SDL_FPoint colliderPos = {
      enemy->position().x + ((float)m_texture->header()->width / 2),
      enemy->position().y + ((float)m_texture->header()->height / 2)};
  enemy->updateCollidePosition(colliderPos);
}

void EnemyManager::spawnEnemies(unsigned count) {
  // TODO: Spawn enemies array
  m_enemies = new Enemy[count];
  m_enemyCount = count;

  SDL_assert(m_enemies != nullptr);

  for (unsigned i = 0; i < count; ++i) {
    Enemy* enemy = &m_enemies[i];
    setEnemyRandomPos(enemy);
  }
}

void EnemyManager::updateState(double delta) {
  SDL_assert(m_enemies != nullptr);

#if 1
  float deltaSeconds = delta / 1000.0f;
  for (unsigned i = 0; i < m_enemyCount; ++i) {
    Enemy* enemy = &m_enemies[i];
    // 만약 현재 위치가 최대 Y 좌표를 벗어난 경우 보이지 않도록 수정
    if(enemy->position().y >= s_enemyMaxYPos) {
      // 보이지 않도록 한 뒤, 스폰 장소로 이동
      enemy->visible(false);
      enemy->state(EnemyStateIdle);
      setEnemyRandomPos(enemy);
    } else {
      enemy->visible(true);
      enemy->state(EnemyStateMove);
      // 이동 좌표
      float yPos = enemy->position().y + enemy->speed() * deltaSeconds;

      enemy->state(EnemyStateMove);

      // 위치 업데이트
      enemy->position({ enemy->position().x, yPos });

      // 충돌체 위치 업데이트
      SDL_FPoint colliderPos = {
        enemy->position().x + ((float)m_texture->header()->width / 2),
        enemy->position().y + ((float)m_texture->header()->height / 2)};
      enemy->setCollider(colliderPos.x, colliderPos.y,
                          s_enemyCircleColliderRadius);

      // TODO: Debug collider points 위치 업데이트
      // 이 위치 이동은 Math::createCirclePoints 보다 빠를 것
      enemy->moveYPosColliderPoints(enemy->speed() * deltaSeconds);
    }
  }
#else
  // 디버그용
  Enemy* e = &m_enemies[0];
  e->position({176, 333});
  SDL_FPoint colliderPos = {
    e->position().x + ((float)m_texture->header()->width / 2),
    e->position().y + ((float)m_texture->header()->height / 2)};
  e->updateCollidePosition(colliderPos);
#endif
}

}  // namespace shmup
