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

namespace {
  int width = 0, height = 0;

  int stride = 0;
  int maxOffset = 0;
}

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

  width = w, height = h;
  stride = width;
  maxOffset = stride * height - 1;

  m_screenBuffer = new RGBA[w * h];
  if(m_screenBuffer == nullptr) {
    return false;
  }

  m_frameTexture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_BGRA32,
                                     SDL_TEXTUREACCESS_STREAMING, w, h);
  if(m_frameTexture == nullptr) {
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

  if(tga.header()->width > 128 && tga.header()->height > 128) {
    std::cout << "TGA texture size is too big to read pixels into the buffer \n";
    return;
  }

  const int pitch = tga.header()->width * sizeof(RGBA);
  // Unknown = Pixel format of the current render target
  SDL_RendererInfo info = {};
  SDL_GetRendererInfo(m_renderer, &info);
  // The format was SDL_PIXELFORMAT_BGRA32 on Windows
  if (SDL_RenderReadPixels(m_renderer, &rect, SDL_PIXELFORMAT_BGRA32, m_pixelBuffer,
                           pitch) != 0) {
    std::cout << "readPixels failed " << SDL_GetError() << std::endl;
    return;
  }
  RGBA* tgaPixels = const_cast<RGBA*>(tga.pixelData());

#if TEST_PREMULTIPLIED_ALPHA
  // Premultiplied blending
  int newX = rect.x + rect.w;
  for (int h = 0; h < tga.header()->height; ++h) {
    for (int w = 0; w < tga.header()->width; ++w) {
      int offset = h * tga.header()->width + w;

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
#else
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
#endif
}

void SDLRenderer::clearColor(RGBA color) {
  for(int y = 0; y < height; ++y) {
    for(int x = 0; x < width; ++x) {
      int offset = y * width + x;
        m_screenBuffer[offset] = { color.b, color.g, color.r, color.a };
    }
  }
}

/*
  (-----width-----)
  +---------------+^
  |               | h
  |   x,y ---+    | i
  |   |      |    | g
  |   +------+ w  | h
  |   h           | t
  +---------------+
*/
void SDLRenderer::renderPixels(const RGBA* src, const SDL_FRect& rect) {
  if(m_screenBuffer == nullptr) return;

  const int dstStart = (int)rect.y * stride + (int)rect.x;
  int srcOffset = 0, dstOffset = 0;

  for (int y = 0; y < rect.h; ++y) {
    for (int x = 0; x < rect.w; ++x) {
      srcOffset = y * rect.w + x;
      dstOffset = dstStart + (y * stride + x);

      if(dstOffset < 0 || dstOffset > maxOffset) { break; }

      RGBA& dest = m_screenBuffer[dstOffset];
      dest = Blend::alpha(src[srcOffset], dest);
    }
  }
}

void SDLRenderer::present() { SDL_RenderPresent(m_renderer); }

void SDLRenderer::flush() { SDL_RenderFlush(m_renderer); }

}  // namespace shmup

