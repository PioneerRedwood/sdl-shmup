//------------------------------------------------------------------------------
// File: StarManager.cpp
// Author: Chris Redwood
// Created: 2024-10-29
// License: MIT License
//------------------------------------------------------------------------------

#include "StarManager.hpp"

#include <iostream>

#include "SDLProgram.hpp"

namespace shmup {

#if _WIN32
constexpr auto s_starFilepath = "../resources/star.tga";
#else
constexpr auto s_starFilepath = "../../resources/star.tga";
#endif

unsigned maxXPos = 0;
unsigned maxYPos = 0;

StarManager::StarManager() {
  m_tga = std::make_unique<TGA>();
  if (m_tga->readFromFile(s_starFilepath) == false) {
    SDL_assert(false);
  }

  if (m_tga->createTexture(SDLProgram::instance()->nativeRenderer()) == false) {
    SDL_assert(false);
  }
  maxXPos = SDLProgram::instance()->width() - m_tga->header()->width;
  maxYPos = SDLProgram::instance()->height() - m_tga->header()->height;
}

StarManager::~StarManager() {
  // for(auto& s : m_stars) {
  //   delete[] s.pixels;
  // }
}

void StarManager::spawnStars(unsigned count) {
  for (unsigned i = 0; i < count; ++i) {
    // rect, speed는 랜덤값으로 주는건 어떤지?
    Star star = {.rect = {0}, .visible = true, .speed = 0};
    star.rect.w =
        (float)(1 * rand() / ((RAND_MAX + 1u) / m_tga->header()->width));
    star.rect.h =
        (float)(1 * rand() / ((RAND_MAX + 1u) / m_tga->header()->height));
    star.rect.x = (float)(rand() / ((RAND_MAX + 1u) / maxXPos));  // 0 ~ maxXPos
    star.speed = (float)(4 + rand() / ((RAND_MAX + 1u) / 8));
    m_stars.push_back(star);
  }
}

void StarManager::updatePositions(float delta) {
  //  std::cout << "Update position: delta " << delta << std::endl;
  for (auto& star : m_stars) {
    // TODO: delta 값을 10.0으로 나눠서 너무 빠르게 움직이는 것을 방지
    star.rect.y += (star.speed * (delta / 16.0f));
    //    star.rect.y += (delta / 16.0f);

    // TODO: 수정바람: 창의 아래로 이동하면 visiable 끄고 x 랜덤한 값으로 변경
    if (star.rect.y >= maxYPos) {
      star.visible = false;
      star.rect.w =
          (float)(1 * rand() / ((RAND_MAX + 1u) / m_tga->header()->width));
      star.rect.h =
          (float)(1 * rand() / ((RAND_MAX + 1u) / m_tga->header()->height));
      star.rect.x = (float)(m_tga->header()->width +
                            rand() / ((RAND_MAX + 1u) / maxXPos));
      star.rect.y = 0;
      star.speed = (float)(4 + rand() / ((RAND_MAX + 1u) / 8));
    } else {
      star.visible = true;
    }
  }
}

}  // namespace shmup
