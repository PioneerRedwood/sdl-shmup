
//------------------------------------------------------------------------------
// File: Player.cpp
// Author: Chris Redwood
// Created: 2024-10-30
// License: MIT License
//------------------------------------------------------------------------------

#include "Player.hpp"

#include <iostream>

#include "Math.hpp"
#include "SDLProgram.hpp"

namespace shmup {

#if _WIN32
constexpr auto s_planeFilepath = "../resources/plane.tga";
constexpr auto s_bulletFilepath = "../resources/bullet.tga";
#else
constexpr auto s_planeFilepath = "../../resources/plane.tga";
constexpr auto s_bulletFilepath = "../../resources/bullet.tga";
#endif

void Bullet::onCollided(const GameObject& target) {
  state = BulletStateHit;
  // TODO: 맞았을 때 처리 
  // m_visible = false;
}

unsigned s_bulletMaxYPos = 0;

SDL_FPoint s_playerSpawnPosition;
int s_playerMoveStepSize = 20;
int s_playerSpeed = 10;

Player::Player() : GameObject() { m_tag = GameObjectTagPlayer; }

Player::~Player() {}

bool Player::loadResource(SDL_Renderer* renderer) {
  // load plane texture
  m_planeTexture = std::make_unique<TGA>();
  if (m_planeTexture->readFromFile(s_planeFilepath) == false) {
    SDL_assert(false);
    return false;
  }

  if (m_planeTexture->createTexture(renderer) == false) {
    SDL_assert(false);
    return false;
  }

  setCollider(0.0f, 0.0f, (float)m_planeTexture->header()->width / 2);

  Math::createCirclePointVector(m_debugColliderPoints, 0.0f, 0.0f,
                                m_planeTexture->header()->width / 2, 90);

  // load bullet texture
  m_bulletTexture = std::make_unique<TGA>();
  if (m_bulletTexture->readFromFile(s_bulletFilepath) == false) {
    SDL_assert(false);
    return false;
  }

  if (m_bulletTexture->createTexture(renderer) == false) {
    SDL_assert(false);
    return false;
  }

  s_bulletMaxYPos =
      SDLProgram::instance()->height() - m_bulletTexture->header()->height;

  m_bulletCount = 30;
  m_bullets = new Bullet[m_bulletCount];

  for (unsigned i = 0; i < m_bulletCount; ++i) {
    Bullet& b = m_bullets[i];

    // 크기, 위치, 충돌체 위치, visible, 스피드 설정
    b.size({10.0f, 10.0f});
    b.position({0.0f, 0.0f});
    b.setCollider(0.0f, 0.0f, 3.0f);
    b.visible(false);
    b.speed = 300.0f;
  }

  return true;
}

void Player::updatePosition(float x, float y) {
  // 현재 좌표 수정
  m_position = {x, y};

  // 플레이어는 반드시 충돌체가 있어야 함
  SDL_assert(m_collider.get() != nullptr);

  // 약간 위쪽으로 위치
  m_collider->pos = {m_position.x, m_position.y + 5.0f};
}

/// @brief 델타 타임을 기반으로 상태 업데이트
/// @param delta
void Player::updateState(double delta) {
  // TODO: 위치 이동
  if (m_queuedMovePositionX != 0.0f) {
    // 부드러운 보간을 위해서 delta 값을 속도에 따른 이동 시간에 맞는 값으로
    // 변경
    float deltaSeconds = delta / 1000.0f;
    float prevXPos = m_position.x;
    m_position.x = m_position.x * (1 - deltaSeconds * s_playerSpeed) +
                   m_queuedMovePositionX * deltaSeconds * s_playerSpeed;

    if (std::fabsf(m_position.x - m_queuedMovePositionX) < 0.001f) {
      m_position.x = m_queuedMovePositionX;
      m_queuedMovePositionX = 0.0f;
    }

    for (auto& p : m_debugColliderPoints) {
      p.x += (prevXPos - m_position.x);
    }

    updatePosition(m_position.x, m_position.y);
  }

  // TODO: 발사
  m_elapsedFireTime += delta;

  // 30밀리초에 한번 씩 발사
  if (m_elapsedFireTime >= 30.0f) {
    std::cout << "Player::updateState fire weapon! " << m_elapsedFireTime << std::endl ;
    // TODO: Fire bullet
    for (unsigned i = 0; i < m_bulletCount; ++i) {
      Bullet& b = m_bullets[i];
      if (b.state == BulletStateIdle) {
        // 총구 위치
        b.position({m_position.x + (m_planeTexture->header()->width / 2), m_position.y - 5.0f});
        b.visible(true);
        b.state = BulletStateFired;
        break;
      }
    }

    m_elapsedFireTime = 0.0f;
  }

  updateBulletPosition(delta);
}

/// @brief 지정한 방향대로 속도와 곱하여 목표 지점을 설정하게 된다
/// @param direction -1.0f ~ 1.0f
void Player::move(int direction) {
  // if(m_queuedMovePositionX == 0.0f) {
  // }
  m_queuedMovePositionX = m_position.x + s_playerMoveStepSize * direction;
  std::cout << "Player::move " << direction << " dst: " << m_queuedMovePositionX
            << std::endl;
}

void Player::spawnBulets(unsigned count) {
  SDL_assert(m_bulletTexture.get() != nullptr);

  if (m_bullets != nullptr) {
    delete[] m_bullets;
  }
  m_bullets = new Bullet[count];
  m_bulletCount = count;
  // memory allocation failed
  SDL_assert(m_bullets != nullptr);

  for (unsigned i = 0; i < count; ++i) {
    Bullet* bullet = &m_bullets[i];
    bullet->setCollider(0.0f, 0.0f, 3);
  }
}

void Player::updateBulletPosition(double delta) {
  SDL_assert(m_bulletTexture.get() != nullptr);
  SDL_assert(m_bullets != nullptr);

  float deltaSeconds = delta / 1000.0f;
  for (unsigned i = 0; i < m_bulletCount; ++i) {
    Bullet& bullet = m_bullets[i];

    if(bullet.state == BulletStateFired) {
      // TODO: 총알은 현재 위치에서 속도에 맞춰 -Y 방향으로 이동(윗쪽)
      float yPos = bullet.position().y - (bullet.speed * deltaSeconds);

      if (yPos <= -10.0f || bullet.hasFlaggedCollided) {
        // TODO: 발사 위치로 이동
        bullet.state = BulletStateIdle;
        bullet.visible(false);
        bullet.position(
            {m_position.x + (m_planeTexture->header()->width / 2), m_position.y});
      } else {
        bullet.visible(true);
        bullet.position({bullet.position().x, yPos});
      }
      // 조금 더 위쪽으로 이동
      bullet.setCollider(bullet.position().x, bullet.position().y, 3);
    }
  }
}

void Player::onCollided(const GameObject& target) {
  // TODO: 맞았음을 가시적으로 보여줘야 함
}

}  // namespace shmup
