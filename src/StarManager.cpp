//------------------------------------------------------------------------------
// File: StarManager.cpp
// Author: Chris Redwood
// Created: 2024-10-29
// License: MIT License
//------------------------------------------------------------------------------

#include "StarManager.hpp"

#include <iostream>

namespace shmup {

#if _WIN32
constexpr auto s_starFilepath = "../resources/star.tga";
#else
constexpr auto s_starFilepath = "../../resources/star.tga";
#endif

unsigned maxXPos = 0;
unsigned maxYPos = 0;

StarManager::StarManager() {}

StarManager::~StarManager() {}

bool StarManager::init(SDL_Renderer* renderer, int width, int height) {
  m_tga = std::make_unique<TGA>();
  if (m_tga->readFromFile(s_starFilepath) == false) {
    std::cout << "StarManager read texture failed \n";
    SDL_assert(false);
    return false;
  }

  if (m_tga->createTexture(renderer) == false) {
    std::cout << "StarManager create texture failed \n";
    SDL_assert(false);
    return false;
  }
  maxXPos = width - m_tga->header()->width;
  maxYPos = height - m_tga->header()->height;
  return true;
}

void StarManager::setStarPropertiesRandomly(Star& star) {
  SDL_FPoint value = {
    (float)(1 * rand() / ((RAND_MAX + 1u) / m_tga->header()->width)),
    (float)(1 * rand() / ((RAND_MAX + 1u) / m_tga->header()->height))
  };
  star.size(value);

  value = {
    (float)(rand() / ((RAND_MAX + 1u) / maxXPos)),  // 0 ~ maxXPos
    (float)(rand() / ((RAND_MAX + 1u) / maxYPos)),  // 0 ~ maxYPos
  };
  star.position(value);

  star.speed = (float)(4 + rand() / ((RAND_MAX + 1u) / 8));
}

void StarManager::spawnStars(unsigned count) {
  for (unsigned i = 0; i < count; ++i) {
    Star star;
    setStarPropertiesRandomly(star);
    m_stars.push_back(star);
  }
}

void StarManager::updatePositions(float delta) {
  //  std::cout << "Update position: delta " << delta << std::endl;
  for (auto& star : m_stars) {
    // 지나치게 빠르게 움직이는 것을 방지
    float yPos = star.position().y + (star.speed * (delta / 16.0f));

    if (yPos >= maxYPos) {
      star.visible(false);
      setStarPropertiesRandomly(star);
    } else {
      star.visible(true);
      star.position({star.position().x, yPos});
    }
  }
}

}  // namespace shmup

