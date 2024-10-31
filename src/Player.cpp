
//------------------------------------------------------------------------------
// File: Player.cpp
// Author: Chris Redwood
// Created: 2024-10-30
// License: MIT License
//------------------------------------------------------------------------------

#include "Player.hpp"

namespace shmup {

#if _WIN32
constexpr auto s_planeFilepath = "../resources/plane.tga";
constexpr auto s_bulletFilepath = "../resources/bullet.tga";
#else
constexpr auto s_planeFilepath = "../../resources/plane.tga";
constexpr auto s_bulletFilepath = "../../resources/bullet.tga";
#endif

SDL_FPoint s_playerSpawnPosition;

Player::Player() : GameObject() {
    m_tag = GameObjectTagPlayer;
}

Player::~Player() {}

bool Player::hasCollider() const {
    return (m_collider != nullptr);
}

bool Player::loadResource(SDL_Renderer* renderer) {
    // load plane texture 
    m_planeTexture = std::make_unique<TGA>();
    if(m_planeTexture->readFromFile(s_planeFilepath) == false) {
        SDL_assert(false);
        return false;
    }

    if(m_planeTexture->createTexture(renderer) == false) {
        SDL_assert(false);
        return false;
    }

    // init collider
    m_collider = std::make_unique<CircleCollider>();
    // 이 충돌체의 좌표는 상대적으로 계산해야 하는데 ..
    m_collider->pos = { 0.0f, 0.0f };
    m_collider->radius = (float)m_planeTexture->header()->width / 2;

    // load bullet texture
    
    return true;
}

void Player::updatePosition(int x, int y) {
    // 현재 좌표 수정
    m_position = { (float)x, (float)y };

    // 충돌체 있으면 충돌체 좌표도 수정
    SDL_assert(m_collider != nullptr);
    
    // 약간 위쪽으로 위치
    m_collider->pos = { m_position.x, m_position.y + 5.0f };
}

void Player::move(double delta) {
  // 이 값은 변경될 수 있음
  // 이동을 부드럽게 하려면 어떻게 해야 하지?
  const float speed = 1.33f;
  updatePosition(m_position.x + delta * speed, m_position.y);
}

}
