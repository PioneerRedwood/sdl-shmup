//------------------------------------------------------------------------------
// File: GameObject.cpp
// Author: Chris Redwood
// Created: 2024-10-30
// License: MIT License
//------------------------------------------------------------------------------

#include "GameObject.hpp"

#include <cmath>
#include <iostream>

#include "Math.hpp"

namespace shmup {

GameObject::GameObject() {}

GameObject::~GameObject() {}

GameObject::GameObject(const GameObject& rhs) {
  m_position = rhs.m_position;
  m_size = rhs.m_size;
  m_tag = rhs.m_tag;
  m_visible = rhs.m_visible;
  if(rhs.m_collider.get() != nullptr) {
    m_collider = std::make_unique<CircleCollider>();
    m_collider->position = rhs.m_collider->position;
    m_collider->radius = rhs.m_collider->radius;
  }
}

GameObject& GameObject::operator=(const GameObject& rhs) {
  m_position = rhs.m_position;
  m_size = rhs.m_size;
  m_tag = rhs.m_tag;
  m_visible = rhs.m_visible;
  if(rhs.m_collider.get() != nullptr) {
    m_collider = std::make_unique<CircleCollider>();
    m_collider->position = rhs.m_collider->position;
    m_collider->radius = rhs.m_collider->radius;
  }
  return *this;
}

GameObject::GameObject(GameObject&& rhs) {
  m_position = rhs.m_position;
  m_size = rhs.m_size;
  m_tag = rhs.m_tag;
  m_visible = rhs.m_visible;
  if(rhs.m_collider.get() != nullptr) {
    m_collider = std::make_unique<CircleCollider>();
    m_collider->position = rhs.m_collider->position;
    m_collider->radius = rhs.m_collider->radius;
  }
  rhs.m_collider.release();
}

GameObject& GameObject::operator=(GameObject&& rhs) {
  m_position = rhs.m_position;
  m_size = rhs.m_size;
  m_tag = rhs.m_tag;
  m_visible = rhs.m_visible;
  if(rhs.m_collider.get() != nullptr) {
    m_collider = std::make_unique<CircleCollider>();
    m_collider->position = rhs.m_collider->position;
    m_collider->radius = rhs.m_collider->radius;
    rhs.m_collider.release();
  }
  return *this;
}

void GameObject::setCollider(float x, float y, float radius) {
  if(!m_collider) {
    m_collider = std::make_unique<CircleCollider>();
  }

  m_collider->position = { x, y };
  m_collider->radius = radius;
}

bool GameObject::hasCollider() const {
    return (m_collider.get() != nullptr);
}

bool GameObject::isCollided(const GameObject& a, const GameObject& b) {
    // 두 객체의 중심점을 바탕으로 충돌했는지 검사

    // 충돌체가 없으면 충돌하지 않은 것으로 간주
    if(a.hasCollider() == false || b.hasCollider() == false) {
        return false;
    }

    // 지정한 태그가 아니라면 충돌 검사를 하지 않음
    // int aTag = static_cast<int>(a.m_tag), bTag = static_cast<int>(b.m_tag);
    // GameObjectTag xorResult = static_cast<GameObjectTag>(aTag ^ bTag);
    int xorResult = a.m_tag ^ b.m_tag;
    if(xorResult == 0x0011 || xorResult == 0x0110) {
      float distance = Math::distance(a.m_collider->position, b.m_collider->position);
      if(distance <= (a.m_collider->radius + b.m_collider->radius)) {
        return true;
      }
      // std::cout << "From A: " << a.m_collider->position.x << ", " << a.m_collider->position.y 
      //           << " To B: " <<  b.m_collider->position.x << ", " << b.m_collider->position.y 
      //           << "=> " << distance
      //           << " & sum of radius " << a.m_collider->radius + b.m_collider->radius << std::endl;
    }
    return false;
}

SDL_FPoint GameObject::getColliderCenterPosition() const {
  // DEBUG assert
  SDL_assert(m_collider.get() != nullptr);

  if(m_collider.get() == nullptr) {
    return {0.0f, 0.0f};
  }

  return m_collider->position;
}

void GameObject::onCollided(const GameObject& target) {}

}
