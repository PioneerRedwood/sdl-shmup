//------------------------------------------------------------------------------
// File: EnemyManager.cpp
// Author: Chris Redwood
// Created: 2024-10-31
// License: MIT License
//------------------------------------------------------------------------------

#include "EnemyManager.hpp"

#include <SDL.h>
#include <iostream>

#include "Math.hpp"

namespace shmup {

#if _WIN32
constexpr auto s_enemyFilepath = "../resources/enemy.tga";
#else
constexpr auto s_enemyFilepath = "../../resources/enemy.tga";
#endif

// 아래 값들은 게임 시작 데이터로 분류 가능
float s_enemyMaxXPos = 0.0f;    // 최대 적 X 좌표
float s_enemyMaxYPos = 0.0f;    // 최대 적 Y 좌표
float s_enemySpeed = 0.1f;     // 밀리초당 이동 거리
unsigned s_enemyMaxCount = 1000;  // 최대 적 개수 (30)
double s_spawnDelay = 10.0f;  // 적 스폰 빈도 수 (밀리초)

EnemyManager::EnemyManager() {}

EnemyManager::~EnemyManager() {
  delete[] m_enemies;
  m_enemies = nullptr;
}

bool EnemyManager::init(SDL_Renderer* renderer, int width, int height) {
  m_texture = new TGA();
  if (m_texture->readFromFile(s_enemyFilepath) == false) {
    std::cout << "EnemyManager read enemy TGA failed \n";
    return false;
  }

  if (m_texture->createTexture(renderer) == false) {
    std::cout << "EnemyManager create enemy texture failed \n";
    return false;
  }

  Enemy::setColliderRadius(m_texture->header()->width / 2);

  s_enemyMaxXPos = width - m_texture->header()->width;
  s_enemyMaxYPos = height + m_texture->header()->height; // 밑에 완전히 사라질 정도

  // 적 배열 처음 생성
  m_enemies = new Enemy[s_enemyMaxCount];
  if(m_enemies == nullptr) {
    std::cout << "EnemyManager allocate pre enemy array failed \n";
    return false;
  }
  m_enemyCount = s_enemyMaxCount;

  // 한번 설정되고 변하지 않는 값만 여기서 설정
  for (unsigned i = 0; i < m_enemyCount; ++i) {
    Enemy* enemy = &m_enemies[i];
    Vector2 value = {(float)m_texture->header()->width,
                     (float)m_texture->header()->height};
    enemy->size(value);
    enemy->setCollider(0.0f, 0.0f, 0.0f);
  }
  return true;
}

void EnemyManager::setEnemyRandomPos(Enemy* enemy) {
  if (m_texture == nullptr) {
    return;
  }

  // x: 0 ~ s_enemyMaxXPos 사이의 값으로 설정
  Vector2 value = {(float)(rand() / ((RAND_MAX + 1u) /
                                     s_enemyMaxXPos)),  
                   0.0f};
  enemy->position(value);

  // 충돌체 위치 업데이트
  Vector2 colliderPos = {
      enemy->position().x + ((float)m_texture->header()->width / 2),
      enemy->position().y + ((float)m_texture->header()->height / 2)};
  enemy->setCollider(colliderPos.x, colliderPos.y, 0.0f);

  // 디버그 충돌체 정점 업데이트
  enemy->moveDebugColliderPoints(colliderPos);
}

/// @brief 적 생성
void EnemyManager::spawnEnemy() {
  if(m_enemies == nullptr) return;

  // Idle 상태인 적 선택
  Enemy* enemy = nullptr;
  for(unsigned i = 0; i < m_enemyCount; ++i) {
    if(m_enemies[i].state() == EnemyStateIdle) {
      enemy = &m_enemies[i];
      break;
    }
  }
  if (enemy == nullptr) return;

  //std::cout << "EnemyManager::spawnEnemy called\n";

  // 위치 변경
  setEnemyRandomPos(enemy);

  // 속도 업데이트
  // enemy->speed((float)(0.01f + rand() / ((RAND_MAX + 1u) / 2)));
  enemy->speed(s_enemySpeed);

  // 적 상태 변경
  enemy->state(EnemyStateMove);

  // 최종 목적지 설정
  enemy->destination({ enemy->position().x, s_enemyMaxYPos });

  enemy->isVisible(true);
}

void EnemyManager::updateState(double delta) {
  if(m_enemies == nullptr) {
    return;
  }

  m_lastTimeEnemySpawned += delta;

  if(m_lastTimeEnemySpawned >= s_spawnDelay) {
    // 새로운 적 스폰
    spawnEnemy();
    m_lastTimeEnemySpawned = 0.0f;
  }

  // 적 배열 돌면서 "움직임" 상태인 적 이동
  for(unsigned i = 0; i < m_enemyCount; ++i) {
    Enemy* enemy = &m_enemies[i];
    switch (enemy->state()) {
#if 1
      case EnemyStateMove: {
        Vector2 currentPos = enemy->position();
        Vector2 direction = (enemy->destination() - currentPos).normalized();
        float magnitude = enemy->speed() * delta;
        Vector2 movement = direction * magnitude;

        // 이동
        enemy->position({ currentPos.x, currentPos.y + movement.y });

        if ((enemy->destination() - enemy->position()).magnitude() < magnitude) {
          // 초기 위치로
          enemy->isVisible(false);
          enemy->state(EnemyStateIdle);
          setEnemyRandomPos(enemy);
        } else {
          // 충돌체 위치 업데이트
          Vector2 colliderPos = {
              enemy->position().x + ((float)m_texture->header()->width / 2),
              enemy->position().y + ((float)m_texture->header()->height / 2)};
          enemy->setCollider(colliderPos.x, colliderPos.y, 0.0f);

          // DEBUG: 이 위치 이동은 Math::createCirclePoints 보다 빠를 것
          for (unsigned i = 0; i < 180; ++i) {
            enemy->debugColliderPoints()[i].y += movement.y;
          }
        }
        break;
      }
#else
      case EnemyStateMove: {
        float newYPos = enemy->position().y + enemy->speed() * delta;
        if(newYPos >= enemy->destination().y) {
          // 도착
          enemy->isVisible(false);
          enemy->state(EnemyStateIdle);
          setEnemyRandomPos(enemy);
        } else {

        }

        break;
      }
#endif
      case EnemyStateHit: {
        // 맞은 것에 대한 처리
        enemy->isVisible(false);
        enemy->state(EnemyStateIdle);
        setEnemyRandomPos(enemy);
        break;
      }
      default: break;
    }
  }
}

}  // namespace shmup
