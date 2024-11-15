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
#include "SDLProgram.hpp"

namespace shmup {

SDLRenderer::SDLRenderer() {}

SDLRenderer::~SDLRenderer() { SDL_DestroyRenderer(m_renderer); }

SDL_Renderer* SDLRenderer::native() { return m_renderer; }

bool SDLRenderer::init(SDL_Window* window, int w, int h) {
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

  m_tempFrameBuffer = SDL_CreateTexture(m_renderer, 
                                        SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, w, h);
  if(m_tempFrameBuffer == nullptr) {
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
  // Unknown = Pixel format of the current render target
  SDL_RendererInfo info = {};
  SDL_GetRendererInfo(m_renderer, &info);
  // The format was SDL_PIXELFORMAT_BGRA32 on Windows
  // TODO: 알파값이 SDL_GetRenderDrawColor 값과 다르다
  if (SDL_RenderReadPixels(m_renderer, &rect, SDL_PIXELFORMAT_BGRA32, m_pixelBuffer,
                           pitch) != 0) {
    std::cout << "readPixels failed " << SDL_GetError() << std::endl;
    return;
  }
  RGBA* tgaPixels = const_cast<RGBA*>(tga.pixelData());

#if 0
  for (int h = 0; h < tga.header()->height; ++h) {
    for (int w = 0; w < tga.header()->width; ++w) {
      int offset = h * tga.header()->width + w;
      RGBA blended = {0};
      switch (m_currentBlendMode) {
        case SDL_BLENDMODE_BLEND: {
          blended = Blend::alpha(tgaPixels[offset], m_pixelBuffer[offset]);
          break;
        }
        case SDL_BLENDMODE_ADD: {
          blended = Blend::additive(tgaPixels[offset], m_pixelBuffer[offset]);
          break;
        }
        case SDL_BLENDMODE_MUL: {
          blended = Blend::multiply(tgaPixels[offset], m_pixelBuffer[offset]);
          break;
        }
        default: {
          blended = tgaPixels[offset];
          break;
        }
      }
      SDL_SetRenderDrawColor(m_renderer, blended.b, blended.g, blended.r,
                             blended.a);
      SDL_RenderDrawPoint(m_renderer, rect.x + w, rect.y + h);
    }
  }
#else
  // Premultiplied blending
  int newX = rect.x + rect.w;
  for (int h = 0; h < tga.header()->height; ++h) {
    for (int w = 0; w < tga.header()->width; ++w) {
      int offset = h * tga.header()->width + w;

      // TODO: 그려지지 않음 확인 요망
      RGBA b1 = Blend::alpha(tgaPixels[offset], m_pixelBuffer[offset]);
      SDL_SetRenderDrawColor(m_renderer, b1.b, b1.g, b1.r,
        b1.a);
      SDL_RenderDrawPoint(m_renderer, rect.x + w, rect.y + h);

      RGBA b2= Blend::premultipliedAlpha(tgaPixels[offset], m_pixelBuffer[offset]);
      SDL_SetRenderDrawColor(m_renderer, b2.b, b2.g, b2.r,
                             b2.a);
      SDL_RenderDrawPoint(m_renderer, newX + w, rect.y + h);

      //printf("TEST alpha x: %d pre alpha: %d \n", rect.x + w, newX + w);
      //printf("TEST alpha %d %d %d %d pre alpha: %d %d %d %d \n", 
      //  b1.r, b1.g, b1.b, b1.a, b2.r, b2.g, b2.b, b2.a);
    }
  }
#endif
}

void SDLRenderer::copyTGAToPixelBuffer(const TGA& tga, int x, int y) {

}

bool SDLRenderer::readPixels(RGBA& pixels, int x, int y, int w, int h) {
  if(w == 0 || h == 0) {
    return false;
  }

  if(w == 0 && h == 0) {
    const int pitch = SDLProgram::instance()->width() * sizeof(RGBA);
    SDL_RenderReadPixels(m_renderer, NULL, SDL_PIXELFORMAT_BGRA32, m_tempFrameBuffer, pitch);
  } else {
    SDL_Rect rect;
    rect.x = x, rect.y = y, rect.w = w, rect.h = h;
    const int pitch = w * sizeof(RGBA);
    SDL_RenderReadPixels(m_renderer, &rect, SDL_PIXELFORMAT_BGRA32, m_tempFrameBuffer, pitch);
  }

  return true;
}

void SDLRenderer::present() { SDL_RenderPresent(m_renderer); }

void SDLRenderer::flush() { SDL_RenderFlush(m_renderer); }

}  // namespace shmup

