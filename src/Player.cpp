
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

unsigned s_bulletMaxYPos = 0;
float s_bulletColliderRadius = 3.0f;
unsigned s_maximumBulletCount = 1;  // 총알 최대 갯수

Vector2 s_playerSpawnPosition;
float s_playerMoveStepSize = 100.0f;
float s_playerSpeed = 1.25f;
float s_playerColliderRadius = 0.0f;
float s_playerFireDelay = 50.0f;
float s_playerMaxXPos = 0.0f;

Player::Player() : GameObject() { 
  m_tag = GameObjectTagPlayer;
  m_isVisible = true;
}

Player::~Player() {
  if (m_planeTexture) {
    delete m_planeTexture;
  }
  if (m_bulletTexture) {
    delete m_bulletTexture;
  }
  if (m_bullets) {
    delete[] m_bullets;
  }
  if (m_debugColliderPoints) {
    delete[] m_debugColliderPoints;
  }
}

bool Player::loadResource(SDL_Renderer* renderer) {
  // load plane texture
  m_planeTexture = new TGA();
  if (m_planeTexture->readFromFile(s_planeFilepath) == false) {
    SDL_assert(false);
    return false;
  }

  if (m_planeTexture->createTexture(renderer) == false) {
    SDL_assert(false);
    return false;
  }
  s_playerColliderRadius = (float)m_planeTexture->header()->width / 4;

  setCollider(0.0f, 0.0f, s_playerColliderRadius);

  m_debugColliderPoints = new Vector2[180];
  Math::createCirclePoints(m_debugColliderPoints, 0.0f, 0.0f,
                           m_planeTexture->header()->width / 2);

  // load bullet texture
  m_bulletTexture = new TGA();
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

  s_playerMaxXPos =
      SDLProgram::instance()->width() - m_planeTexture->header()->width;

  // 임시로 총알 갯수 제한
  m_bulletCount = s_maximumBulletCount;
  m_bullets = new Bullet[m_bulletCount];

  return true;
}

void Player::updatePosition(float x, float y) {
  // 현재 좌표 수정
  m_position = {x, y};

  // 플레이어는 반드시 충돌체가 있어야 함
  Vector2 colliderPos = {
      x + m_planeTexture->header()->width / 2,
      y + m_planeTexture->header()->height / 2,
  };
  setCollider(colliderPos.x, colliderPos.y, s_playerColliderRadius);

  // 충돌체 원형 좌표 수정
  Math::createCirclePoints(m_debugColliderPoints, colliderPos.x, colliderPos.y,
                           s_playerColliderRadius);
}

/// @brief 델타 타임을 기반으로 상태 업데이트
/// @param delta
void Player::updateState(double delta) {
  // 위치 이동
  if (m_isFlaggedToMove) {
    float prevXPos = m_position.x;

    // 방향 구하기
    Vector2 direction = (m_destPos - m_position).normalized();
    // 해당 프레임에서 얼마나 이동해야 하는지 구하기
    float magnitude = s_playerSpeed * delta;
    Vector2 movement = direction * magnitude;

    m_position.x = m_position.x + movement.x;

    if ((m_destPos - m_position).magnitude() < magnitude) {
      m_position.x = m_destPos.x;
      m_destPos.x = 0.0f;
      m_isFlaggedToMove = false;
    }

    for (unsigned i = 0; i < 180; ++i) {
      Vector2* p = &m_debugColliderPoints[i];
      p->x += (prevXPos - m_position.x);
    }

    updatePosition(m_position.x, m_position.y);
  }

  m_elapsedFireTime += delta;

  if (m_elapsedFireTime >= s_playerFireDelay) {
    // std::cout << "Player::updateState fire weapon! " << m_elapsedFireTime <<
    // std::endl ;
    for (unsigned i = 0; i < m_bulletCount; ++i) {
      Bullet* b = &m_bullets[i];
      if (b->state() == BulletStateIdle) {
        Vector2 pos = {m_position.x + (m_planeTexture->header()->width / 2),
                       m_position.y - 5.0f};
        b->position(pos);
        b->isVisible(true);
        pos = {b->position().x + b->size().x / 2,
               b->position().y + b->size().y / 2};
        b->setCollider(pos.x, pos.y, s_bulletColliderRadius);
        b->state(BulletStateFired);
        b->destination({pos.x, -10.0f});
        Math::createCirclePoints(b->debugPoints(), pos.x, pos.y,
                                 s_bulletColliderRadius);
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
  // const float maxGapFromEdge = 5.0f;
  const float maxGapFromEdge = m_planeTexture->header()->width;
  if (m_position.x <= 0.0f) {
    m_isFlaggedToMove = true;
    m_destPos.x = m_position.x + maxGapFromEdge;
    return;
  }

  if (m_position.x >= s_playerMaxXPos) {
    m_isFlaggedToMove = true;
    m_destPos.x = m_position.x - maxGapFromEdge;
    return;
  }

  m_isFlaggedToMove = true;
  m_destPos.x = m_position.x + s_playerMoveStepSize * direction;

  std::cout << "Player::move called " << m_position.x << " > " << m_destPos.x
            << std::endl;
}

void Player::updateBulletPosition(double delta) {
  if (m_bulletTexture == nullptr || m_bullets == nullptr) {
    return;
  }
  const float deltaSeconds = delta / 1000.0f;
  for (unsigned i = 0; i < m_bulletCount; ++i) {
    Bullet* bullet = &m_bullets[i];
#if 1
    if(bullet->state() == BulletStateFired) {
      float newYPos = bullet->position().y - bullet->speed() * delta;
      
      if(newYPos <= bullet->destination().y) {
        // 도착
        bullet->state(BulletStateIdle);
        bullet->isVisible(false);
        
        // 총구 위치로 이동
        bullet->position({m_position.x + (m_planeTexture->header()->width / 2), m_position.y});

        // 총알 충돌체 이동
        Vector2 newColliderPos = {
          bullet->position().x + bullet->size().x / 2,
          bullet->position().y + bullet->size().y / 2
        };
        bullet->setCollider(newColliderPos.x, newColliderPos.y, s_bulletColliderRadius);
        
        // 디버그 포인트 이동
        Math::createCirclePoints(bullet->debugPoints(), newColliderPos.x, newColliderPos.y, s_bulletColliderRadius);
      } else {
        // 이동
        // 총알은 현재 위치에서 속도에 맞춰 -Y 방향으로 이동(윗쪽)
        float deltaYPos = bullet->position().y;
        
        bullet->position({ bullet->position().x, newYPos });

        // 가장 근사한 위치로 충돌체 이동
       Vector2 newColliderPos = {
          bullet->position().x + bullet->size().x / 2, 
          bullet->position().y + bullet->size().y / 2
        };
        bullet->setCollider(newColliderPos.x, newColliderPos.y, s_bulletColliderRadius);

        deltaYPos = deltaYPos - newYPos;
        for (unsigned i = 0; i < 180; ++i) {
          bullet->debugPoints()[i].y -= deltaYPos;
        }
      }
    }
#else
    switch (bullet->state()) {
      case BulletStateFired: {
        Vector2 currentPos = bullet->position();
        Vector2 direction = (bullet->destination() - currentPos).normalized();
        float magnitude = bullet->speed() * delta;
        Vector2 movement = direction * magnitude;

        bullet->position({currentPos.x, currentPos.y + movement.y});

        if ((bullet->destination() - bullet->position()).magnitude() <
            magnitude) {
          // 초기 위치로
          bullet->isVisible(false);
          bullet->state(BulletStateIdle);
          Vector2 newPos = {m_position.x + (m_planeTexture->header()->width / 2),
                            m_position.y};
          bullet->position(newPos);
          newPos = {bullet->position().x + bullet->size().x / 2,
                    bullet->position().y + bullet->size().y / 2};
          bullet->setCollider(newPos.x, newPos.y, s_bulletColliderRadius);
          Math::createCirclePoints(bullet->debugPoints(), newPos.x, newPos.y,
                                  s_bulletColliderRadius);
        } else {
          // 충돌체 위치 업데이트
          Vector2 newPos = {
              bullet->position().x + (m_bulletTexture->header()->width / 4),
              bullet->position().y + (m_bulletTexture->header()->height / 4)};
          bullet->setCollider(newPos.x, newPos.y, s_bulletColliderRadius);

          for (unsigned i = 0; i < 180; ++i) {
            bullet->debugPoints()[i].y += movement.y;
          }
        }
        break;
      }
      default: {
        break;
      }
    }
#endif
  }
}

void Player::onCollided(const GameObject& target) {
  // TODO: 맞았음을 가시적으로 보여줘야 함
}

Vector2 Player::getColliderCenterByDelta(double delta) const {
  if (m_isFlaggedToMove) {
    float magnitude = s_playerSpeed * delta;
    Vector2 pos = {m_collider->position.x + magnitude, m_collider->position.y};

    // 목적지에 도착을 했으면 목적지 위치의 충돌체 중심 좌표를 반환
    if ((m_destPos - pos).magnitude() <= magnitude) {
      return {m_destPos.x + m_planeTexture->header()->width / 2,
              m_destPos.y + m_planeTexture->header()->height / 2};
    } else {
      return pos;
    }
  }
  return m_collider->position;
}

}  // namespace shmup
