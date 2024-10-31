//------------------------------------------------------------------------------
// File: Player.hpp
// Author: Chris Redwood
// Created: 2024-10-30
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include <SDL.h>
#include <memory>
#include "TGA.hpp"
#include "GameObject.hpp"

namespace shmup {

class Player : public GameObject {
public:
    Player();

    ~Player();

    bool loadResource(SDL_Renderer* renderer);

    void updatePosition(int x, int y);
    
    void move(double delta);

    bool hasCollider() const override;

    inline std::unique_ptr<TGA>& plane() {
      return m_planeTexture;
    }

    inline std::unique_ptr<TGA>& bullet() {
      return m_bulletTexture;
    }

private:
    std::unique_ptr<TGA> m_planeTexture;

    std::unique_ptr<TGA> m_bulletTexture;

    unsigned m_hp;
};

}
