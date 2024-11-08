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

float s_starMaxXPos = 0.0f;
float s_starMaxYPos = 0.0f;

StarManager::StarManager() { m_starSpawnDelay = 100.0f; }

StarManager::~StarManager() {
  if (m_tga) {
    delete m_tga;
  }
  if (m_stars) {
    delete[] m_stars;
  }
}

bool StarManager::init(SDL_Renderer* renderer, int width, int height,
                       unsigned starCount) {
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

  // 스타 생성
  m_starCount = starCount;
  m_stars = new Star[m_starCount];
  if (m_stars == nullptr) {
    std::cout << "StarManager create star array failed \n";
    return false;
  }
  return true;
}

void StarManager::setStarRandomPos(Star* star) {
  Vector2 value = {
      (float)(1 * rand() / ((RAND_MAX + 1u) / m_tga->header()->width)),
      (float)(1 * rand() / ((RAND_MAX + 1u) / m_tga->header()->height))};
  star->size(value);

  value = {
      (float)(rand() / ((RAND_MAX + 1u) / s_starMaxXPos)),  // 0.0f ~ s_starMaxXPos
      (float)(-100.0f + rand() / ((RAND_MAX + 1u))),  // -100.0f ~ 0.0
  };
  star->position(value);

  star->speed = (float)(1.0f + rand() / ((RAND_MAX + 1u) / 2));  // 1.0f ~ 3.0f
}

void StarManager::updateState(float delta) {
  if (m_stars == nullptr) return;

  // 정해진 시간이 지나면 스타 연출 시작
  m_lastStarSpawnTime += delta;
  if (m_lastStarSpawnTime >= m_starSpawnDelay) {
    for (unsigned i = 0; i < m_starCount; ++i) {
      Star* star = &m_stars[i];
      if (star->isVisible() == false) {
        star->isVisible(true);
        setStarRandomPos(star);
        break;
      }
    }
    m_lastStarSpawnTime = 0.0f;
  }

  // 스타 연출 업데이트
  for (unsigned i = 0; i < m_starCount; ++i) {
    Star* star = &m_stars[i];
    if (star->isVisible() == true) {
      float newYPos = star->position().y + star->speed * delta;

      // 목표한 지점에 도달하면 보이지 않도록
      if (newYPos > s_starMaxYPos) {
        star->isVisible(false);
      } else {
        star->isVisible(true);
        star->position({star->position().x, newYPos});
      }
    }
  }
}

const TGA& StarManager::tga() { return *m_tga; }

const Star* StarManager::stars() { return m_stars; }

unsigned StarManager::starCount() const { return m_starCount; }

}  // namespace shmup
