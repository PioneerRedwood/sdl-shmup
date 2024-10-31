//------------------------------------------------------------------------------
// File: GameObject.cpp
// Author: Chris Redwood
// Created: 2024-10-30
// License: MIT License
//------------------------------------------------------------------------------

#include "GameObject.hpp"
#include <cmath>
#include "Math.hpp"

namespace shmup {

GameObject::GameObject() {}

GameObject::~GameObject() {}

bool GameObject::hasCollider() const {
    return (m_collider != nullptr);
}

SDL_FPoint GameObject::position() const {
    return m_pos;
}

GameObjectTag GameObject::tag() const {
    return m_tag;
}

bool GameObject::isCollided(const GameObject& a, const GameObject& b) {
    // 두 객체의 중심점을 바탕으로 충돌했는지 검사

    // 충돌체가 없으면 충돌하지 않은 것으로 간주
    if(a.hasCollider() == false || b.hasCollider() == false) {
        return false;
    }

    // 지정한 태그가 아니라면 충돌 검사를 하지 않음
    uint16_t detectedTag = a.m_tag ^ b.m_tag;
    if(detectedTag == 0x0000) {
        return false;
    }

    float distance = math::distance(a.m_collider->pos, b.m_collider->pos);
    if(distance <= (a.m_collider->radius + a.m_collider->radius)) {
        return true;
    }

    return false;
}

}