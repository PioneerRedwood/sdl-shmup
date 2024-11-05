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

  bool hasCollider() const;

  Vector2 getColliderCenterPosition() const;

  void tag(GameObjectTag tag) { m_tag = tag; }
  
  GameObjectTag tag() const { return m_tag; }

  Vector2 position() const;

  void position(Vector2 pos);

  Vector2 size() const { return m_size; }

  void size(Vector2 size) { m_size = size; }

  bool isVisible() const { return m_isVisible; }

  void isVisible(bool value) { m_isVisible = value; }

  virtual void onCollided(const GameObject& target) = 0;

  /// @brief 계산을 통해 두 오브젝트가 충돌했는지 확인
  static bool isCollided(const GameObject& a, const GameObject& b);

 protected:
  Vector2 m_position;

  Vector2 m_size;

  GameObjectTag m_tag = GameObjectTagNone;

  bool m_isVisible = true;

  CircleCollider* m_collider = nullptr;
};
}  // namespace shmup
