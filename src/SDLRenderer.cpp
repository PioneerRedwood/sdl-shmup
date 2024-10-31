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
  return true;
}

void SDLRenderer::clear() { SDL_RenderClear(m_renderer); }

void SDLRenderer::enableBlending(SDL_BlendMode blendMode) {
  m_currentBlendMode = blendMode;
}

void SDLRenderer::disableBlending() { m_currentBlendMode = SDL_BLENDMODE_NONE; }

void SDLRenderer::drawTGA(std::unique_ptr<TGA>& tga, int x, int y) {
  SDL_Rect rect = {0};
  rect.x = x, rect.y = y, rect.w = tga->header()->width,
  rect.h = tga->header()->height;

  if (m_currentBlendMode == SDL_BLENDMODE_NONE) {
    SDL_RenderCopy(m_renderer, const_cast<SDL_Texture*>(tga->sdlTexture()),
                   nullptr, &rect);
    return;
  }

  if (tga->pixelData() == nullptr) {
    std::cout << "TGA does not have any available pixels \n";
    return;
  }

  RGBA* pixels = new RGBA[tga->header()->width * tga->header()->height];
  if (pixels == nullptr) {
    return;
  }
  const int pitch = tga->header()->width * sizeof(RGBA);

  if (SDL_RenderReadPixels(m_renderer, &rect, SDL_PIXELFORMAT_BGRA32, pixels,
                           pitch) != 0) {
    std::cout << "readPixels failed " << SDL_GetError() << std::endl;
    return;
  }
  RGBA* tgaPixels = const_cast<RGBA*>(tga->pixelData());
  for (int h = 0; h < tga->header()->height; ++h) {
    for (int w = 0; w < tga->header()->width; ++w) {
      int offset = h * tga->header()->width + w;
      // TODO: Do the math here
      RGBA blended = {0};
      switch (m_currentBlendMode) {
        case SDL_BLENDMODE_BLEND: {
          blended = blend::alpha(tgaPixels[offset], pixels[offset]);
          break;
        }
        case SDL_BLENDMODE_ADD: {
          blended = blend::additive(tgaPixels[offset], pixels[offset]);
          break;
        }
        case SDL_BLENDMODE_MUL: {
          blended = blend::multiply(tgaPixels[offset], pixels[offset]);
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
  delete[] pixels;
}

void SDLRenderer::present() { SDL_RenderPresent(m_renderer); }

void SDLRenderer::flush() { SDL_RenderFlush(m_renderer); }

}  // namespace shmup

