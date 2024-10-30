//------------------------------------------------------------------------------
// File: GameObject.hpp
// Author: Chris Redwood
// Created: 2024-10-30
// License: MIT License
//------------------------------------------------------------------------------

#include <SDL.h>

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
    virtual bool hasCollider() const;
    
    GameObjectTag tag() const;
    
    SDL_FPoint position() const;

    /// @brief 어떤 태그끼리 충돌 검사를 수행해야 하는지에 대해 초기화
    // static void initCollisionInfo();

    /// @brief 계산을 통해 두 오브젝트가 충돌했는지 확인
    static bool isCollided(const GameObject& a, const GameObject& b);

protected:
    SDL_FPoint m_pos;

    GameObjectTag m_tag = GameObjectTagNone;

    bool m_visible = true;

    CircleCollider* m_collider = nullptr;
};
}
