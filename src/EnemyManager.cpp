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

void Enemy::onCollided(const GameObject& target) {
  switch(target.tag()) {
    case GameObjectTagPlayer: {
      break;
    }
    case GameObjectTagBullet: {
      break;
    }
    default: {
      break;
    }
  }
}

EnemyManager::EnemyManager() {}

EnemyManager::~EnemyManager() {
  delete[] m_enemies;
  m_enemies = nullptr;
}

bool EnemyManager::init(SDL_Renderer* renderer, int width, int height) {
  // TODO: Load texture
  m_texture = std::make_unique<TGA>();
  if(m_texture->readFromFile(s_enemyFilepath) == false) {
    SDL_assert(false);
    return false;
  }

  // TODO: Create texture
  if(m_texture->createTexture(renderer) == false) {
    SDL_assert(false);
    return false;
  }

  s_enemyMaxXPos = width - m_texture->header()->width;
  s_enemyMaxYPos = height - m_texture->header()->height;

  return true;
}

void EnemyManager::setEnemyRandomPos(Enemy* enemy) {
  SDL_assert(m_texture.get() != nullptr);

  SDL_FPoint value = {
    (float)m_texture->header()->width, (float)m_texture->header()->height
  };
  enemy->size(value);

  value = {
    (float)(rand() / ((RAND_MAX + 1u) / s_enemyMaxXPos)),  // 0 ~ s_enemyMaxXPos
    0.0f
  };
  enemy->position(value);
  
  // TODO: 디버그 포인트 위치 업데이트
  for(unsigned i = 0; i < enemy->debugColliderPoints.size(); ++i) {
    enemy->debugColliderPoints[i].y = value.y;
  }

  enemy->speed = (float)(10.0f + rand() / ((RAND_MAX + 1u) / 25));
}

void EnemyManager::spawnEnemies(unsigned count) {
  // TODO: Spawn enemies array
  m_enemies = new Enemy[count];
  m_enemyCount = count;

  SDL_assert(m_enemies != nullptr);

  for(unsigned i = 0; i < count; ++i) {
    Enemy* enemy = &m_enemies[i];
    setEnemyRandomPos(enemy);
    Math::createCirclePointVector(enemy->debugColliderPoints, enemy->position().x, enemy->position().y, (float)m_texture->header()->width, 18);
    enemy->setCollider(enemy->position().x, enemy->position().y, 2);
  }
}

void EnemyManager::updateState(double delta) {
  // TODO: Update positions of enemies
  SDL_assert(m_enemies != nullptr);

  float deltaSeconds = delta / 1000.0f;
  for (unsigned i = 0; i < m_enemyCount; ++i) {
    Enemy& enemy = m_enemies[i];
    float yPos = enemy.position().y + enemy.speed * deltaSeconds;

    if (yPos >= s_enemyMaxYPos) {
      enemy.visible(false);
      setEnemyRandomPos(&enemy);
      enemy.setCollider(enemy.position().x, enemy.position().y, 2);
    } else {
      enemy.visible(true);
      enemy.position({enemy.position().x, yPos});
      enemy.setCollider(enemy.position().x, enemy.position().y, 2);
    }

    // TODO: debug collider points 위치 업데이트
    for(unsigned i = 0; i < enemy.debugColliderPoints.size(); ++i) {
      enemy.debugColliderPoints[i].y += enemy.speed * deltaSeconds;
    }
  }
}


}
