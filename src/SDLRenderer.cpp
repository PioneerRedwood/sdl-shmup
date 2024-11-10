//------------------------------------------------------------------------------
// File: SDLRenderer.cpp
// Author: Chris Redwood
// Created: 2024-10-21
// License: MIT License
//------------------------------------------------------------------------------

#include "SDLRenderer.hpp"

#include <algorithm>
#include <iostream>  // ste::cout long

#include "Blend.hpp"

namespace shmup {

SDLRenderer::SDLRenderer() {}

SDLRenderer::~SDLRenderer() { SDL_DestroyRenderer(m_renderer); }

SDL_Renderer* SDLRenderer::native() { return m_renderer; }

bool SDLRenderer::init(SDL_Window* window) {
  m_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (m_renderer == nullptr) {
    std::cout << "SDL_CreateRenderer failed error: " << SDL_GetError() << std::endl;
    return false;
  }

  // 전체 프레임의 픽셀을 복사할 수 있을 만한 크기의 버퍼를 생성
  // 크기는 지정한 최대 텍스처 크기인 sizeof(RGBA) 4 * 128 * 128 = 64 KB
  m_pixelBuffer = new RGBA[128 * 128];
  if(m_pixelBuffer == nullptr) {
    return false;
  }
  return true;
}

void SDLRenderer::clear() { SDL_RenderClear(m_renderer); }

void SDLRenderer::enableBlending(SDL_BlendMode blendMode) {
  m_currentBlendMode = blendMode;
}

void SDLRenderer::disableBlending() { m_currentBlendMode = SDL_BLENDMODE_NONE; }

void SDLRenderer::drawTGA(const TGA& tga, int x, int y) {
  SDL_Rect rect = {0};
  rect.x = x, rect.y = y, rect.w = tga.header()->width,
  rect.h = tga.header()->height;

  if (m_currentBlendMode == SDL_BLENDMODE_NONE) {
    SDL_RenderCopy(m_renderer, const_cast<SDL_Texture*>(tga.sdlTexture()),
                   nullptr, &rect);
    return;
  }

  if (tga.pixelData() == nullptr) {
    std::cout << "TGA does not have any available pixels \n";
    return;
  }

  // 만약 미리 정해놓은 텍스처 최대 크기 보다 텍스처가 크면?
  if(tga.header()->width > 128 && tga.header()->height > 128) {
    std::cout << "TGA texture size is too big to read pixels into the buffer \n";
    // 그냥 텍스처로 그리기?
    return;
  }

  const int pitch = tga.header()->width * sizeof(RGBA);
  if (SDL_RenderReadPixels(m_renderer, &rect, SDL_PIXELFORMAT_BGRA32, m_pixelBuffer,
                           pitch) != 0) {
    std::cout << "readPixels failed " << SDL_GetError() << std::endl;
    return;
  }
  RGBA* tgaPixels = const_cast<RGBA*>(tga.pixelData());
  for (int h = 0; h < tga.header()->height; ++h) {
    for (int w = 0; w < tga.header()->width; ++w) {
      int offset = h * tga.header()->width + w;
      RGBA blended = {0};
      switch (m_currentBlendMode) {
        case SDL_BLENDMODE_BLEND: {
          blended = blend::alpha(tgaPixels[offset], m_pixelBuffer[offset]);
          break;
        }
        case SDL_BLENDMODE_ADD: {
          blended = blend::additive(tgaPixels[offset], m_pixelBuffer[offset]);
          break;
        }
        case SDL_BLENDMODE_MUL: {
          blended = blend::multiply(tgaPixels[offset], m_pixelBuffer[offset]);
          break;
        }
        default: {
          blended = tgaPixels[offset];
          break;
        }
      }
      // RGBA 480 * 640 buffer write
      // DrawPoints 한번만
      SDL_SetRenderDrawColor(m_renderer, blended.b, blended.g, blended.r,
                             blended.a);
      SDL_RenderDrawPoint(m_renderer, rect.x + w, rect.y + h);
    }
  }
}

void SDLRenderer::present() { SDL_RenderPresent(m_renderer); }

void SDLRenderer::flush() { SDL_RenderFlush(m_renderer); }

}  // namespace shmup

