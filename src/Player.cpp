
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
unsigned s_maximumBulletCount = 25;  // 총알 최대 갯수

float s_playerSpeed = 0.25f;
float s_playerColliderRadius = 0.0f;
float s_playerFireDelay = 100.0f;
float s_playerMaxXPos = 0.0f;

///////////////////////////////////////////////////////////////
/// Player
///////////////////////////////////////////////////////////////

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

void Player::fire()
{
  // std::cout << "Player::updateState fire weapon! " << m_elapsedFireTime <<
  // std::endl ;
  for (unsigned i = 0; i < m_bulletCount; ++i)
  {
    Bullet *b = &m_bullets[i];
    if (b->state() == BulletStateIdle)
    {
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

      m_fired.push_back(b);
      break;
    }
  }
}

/// @brief 델타 타임을 기반으로 상태 업데이트
/// @param delta
void Player::updateState(double delta) {
  // 위치 이동
  if (m_directionToMoveThisFrame) {
    m_position.x += s_playerSpeed * delta * m_directionToMoveThisFrame;
    updatePosition(m_position.x, m_position.y);
  }
  
  // 총알 발사
  m_elapsedFireTime += delta;
  if (m_elapsedFireTime >= s_playerFireDelay) {
    fire();
    m_elapsedFireTime = 0.0f;
  }

  // 총알 업데이트
  updateBullets(delta);
}

/// @brief 지정한 방향대로 속도와 곱하여 목표 지점을 설정하게 된다
/// @param direction -1.0f, 1.0f
void Player::move(int direction) {

#if 0
  const float gap = 10.0f;
  if (m_position.x <= gap || m_position.x >= (s_playerMaxXPos - gap)) {
    m_directionToMoveThisFrame = 0;
    return;
  }
#endif

  m_directionToMoveThisFrame = direction;

  //std::cout << "Player::move called " << m_position.x << " > " << m_destPos.x
  //          << std::endl;
}

void Player::updateBullets(double delta) {
  if (m_bulletTexture == nullptr || m_bullets == nullptr) {
    return;
  }
  const float deltaSeconds = delta / 1000.0f;
  std::list<Bullet*>::iterator bulletIter = m_fired.begin();
  while(bulletIter != m_fired.end()) {
    Bullet* bullet = (*bulletIter);
    float movement = bullet->speed() * delta;
    float newYPos = bullet->position().y - movement;
    
    if(newYPos <= bullet->destination().y) {
      // 도착
      bullet->isVisible(false);
      bullet->state(BulletStateIdle);
      bulletIter = m_fired.erase(bulletIter);
    } else {
      // 이동
      bullet->position({bullet->position().x, bullet->position().y - movement});

      Vector2 newColliderPos = {
        bullet->position().x + bullet->size().x / 2, 
        bullet->position().y + bullet->size().y / 2
      };
      bullet->setCollider(newColliderPos.x, newColliderPos.y, s_bulletColliderRadius);

      for (unsigned i = 0; i < 180; ++i) {
        bullet->debugPoints()[i].y -= movement;
      }
      ++bulletIter;
    }
  }
}

void Player::onCollided(const GameObject& target) {
  // TODO: 맞았음을 가시적으로 보여줘야 함
  std::cout << "Player::onCollided with enemy! \n";
}

Vector2 Player::getColliderCenterByDelta(double delta) const {
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

}  // namespace shmup
