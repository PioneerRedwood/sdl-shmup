
//------------------------------------------------------------------------------
// File: GameObject.hpp
// Author: Chris Redwood
// Created: 2024-10-30
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include <SDL.h>

#include <memory>

namespace shmup {

struct CircleCollider {
  SDL_FPoint pos;
  float radius;
};

enum GameObjectTag : uint16_t {
  GameObjectTagNone = 0x0000,
  GameObjectTagPlayer = 0x001,
  GameObjectTagEnemy = 0x010,
  GameObjectTagBullet = 0x001,
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

  virtual bool hasCollider() const;

  inline GameObjectTag tag() const { return m_tag; }

  inline SDL_FPoint position() const { return m_position; }

  inline void position(SDL_FPoint pos) { m_position = pos; }

  inline SDL_FPoint size() const { return m_size; }

  inline void size(SDL_FPoint size) { m_size = size; }

  inline bool visible() const { return m_visible; }

  inline void visible(bool value) { m_visible = value; }

  /// @brief 어떤 태그끼리 충돌 검사를 수행해야 하는지에 대해 초기화
  // static void initCollisionInfo();

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
