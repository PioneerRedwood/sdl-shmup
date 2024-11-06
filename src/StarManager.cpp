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

unsigned s_starMaxXPos = 0;
unsigned s_starMaxYPos = 0;

StarManager::StarManager() {}

StarManager::~StarManager() {
  if(m_tga) {
    delete m_tga;
  }
  if(m_stars) {
    delete[] m_stars;
  }
}

bool StarManager::init(SDL_Renderer* renderer, int width, int height) {
  m_tga = new TGA();
  if (m_tga->readFromFile(s_starFilepath) == false) {
    std::cout << "StarManager read texture failed \n";
    return false;
  }

  if (m_tga->createTexture(renderer) == false) {
    std::cout << "StarManager create texture failed \n";
    return false;
  }
  s_starMaxXPos = width - m_tga->header()->width;
  s_starMaxYPos = height - m_tga->header()->height;
  return true;
}

void StarManager::setStarRandomPos(Star* star) {
  Vector2 value = {
    (float)(1 * rand() / ((RAND_MAX + 1u) / m_tga->header()->width)),
    (float)(1 * rand() / ((RAND_MAX + 1u) / m_tga->header()->height))
  };
  star->size(value);

  value = {
    (float)(rand() / ((RAND_MAX + 1u) / s_starMaxXPos)),  // 0 ~ s_starMaxXPos
    (float)(rand() / ((RAND_MAX + 1u) / s_starMaxYPos)),  // 0 ~ s_starMaxYPos
  };
  star->position(value);

  star->speed = (float)(4 + rand() / ((RAND_MAX + 1u) / 8));
}

void StarManager::spawnStars(unsigned count) {
  m_stars = new Star[count];
  m_starCount = count;

  SDL_assert(m_stars != nullptr);
  
  for (unsigned i = 0; i < count; ++i) {
    Star* star = &m_stars[i];
    star->isVisible(true);
    setStarRandomPos(star);
  }
}

void StarManager::updateState(float delta) {
  if (m_stars == nullptr) return;

  //  std::cout << "Update position: delta " << delta << std::endl;
  for (unsigned i = 0; i < m_starCount; ++i) {
    Star* star = &m_stars[i];
    float yPos = star->position().y + (star->speed * (delta / 16.0f));

    if (yPos >= s_starMaxYPos) {
      star->isVisible(false);
      setStarRandomPos(star);
    } else {
      star->isVisible(true);
      star->position({star->position().x, yPos});
    }
  }
}

const TGA& StarManager::tga() {
  return *m_tga;
}

const Star* StarManager::stars() {
  return m_stars;
}

unsigned StarManager::starCount() const {
  return m_starCount;
}

}  // namespace shmup

