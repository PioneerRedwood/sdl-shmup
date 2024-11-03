//------------------------------------------------------------------------------
// File: GameObject.hpp
// Author: Chris Redwood
// Created: 2024-10-30
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include <SDL.h>

#include <memory>
#include "CircleCollider.hpp"

namespace shmup {

enum GameObjectTag : int {
  GameObjectTagNone = 0x0000,
  GameObjectTagPlayer = 0x0001,
  GameObjectTagEnemy = 0x0010,
  GameObjectTagBullet = 0x0100,
};

class GameObject {
 protected:
  GameObject();

  ~GameObject();

 public:
  GameObject(const GameObject& rhs);
  GameObject& operator=(const GameObject& rhs);

  GameObject(GameObject&& rhs);
  GameObject& operator=(GameObject&& rhs);
  
  virtual void setCollider(float x, float y, float radius);

  virtual bool hasCollider() const;

  SDL_FPoint getColliderCenterPosition() const;

  inline void tag(GameObjectTag tag) { m_tag = tag; }
  
  inline GameObjectTag tag() const { return m_tag; }

  inline SDL_FPoint position() const { return m_position; }

  inline void position(SDL_FPoint pos) { m_position = pos; }

  inline SDL_FPoint size() const { return m_size; }

  inline void size(SDL_FPoint size) { m_size = size; }

  inline bool visible() const { return m_visible; }

  inline void visible(bool value) { m_visible = value; }

  virtual void onCollided(const GameObject& target);

  /// @brief 계산을 통해 두 오브젝트가 충돌했는지 확인
  static bool isCollided(const GameObject& a, const GameObject& b);

 protected:
  SDL_FPoint m_position;

  SDL_FPoint m_size;

  GameObjectTag m_tag = GameObjectTagNone;

  bool m_visible = true;

  std::unique_ptr<CircleCollider> m_collider;
};
}  // namespace shmup
