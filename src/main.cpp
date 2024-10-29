#include <iostream>
#include <memory>
#include "TGA.hpp"
#include <SDL.h>
#include "SDLProgram.hpp"

#if _WIN32
constexpr auto s_backgroundFilepath = "../resources/bg.tga";
constexpr auto s_particleFilepath = "../resources/particle2.tga";
#else
constexpr auto s_backgroundFilepath = "../../resources/bg.tga";
constexpr auto s_particleFilepath = "../../resources/particle2.tga";
#endif

int main(int argc, char** argv) {
    std::unique_ptr<SDLProgram> program = std::make_unique<SDLProgram>();

    if(program->init(480, 640) == false) {
        return 1;
    }

    // 리소스 로드
    std::unique_ptr<TGA> bg = std::make_unique<TGA>();
    if(bg->readFromFile(s_backgroundFilepath) == false) {
        std::cout << "File read failed " << s_backgroundFilepath << std::endl;
        return 1;
    }
    if(bg->createTexture(program->nativeRenderer()) == false) {
        std::cout << "Create texture failed " << s_backgroundFilepath << std::endl;
        return 1;
    }
    std::unique_ptr<TGA> particle = std::make_unique<TGA>();
    if(particle->readFromFile(s_particleFilepath) == false) {
        std::cout << "File read failed " << s_particleFilepath << std::endl;
        return 1;
    }
    if(particle->createTexture(program->nativeRenderer()) == false) {
        return 1;
    }
    auto& renderer = program->renderer();
    auto* nativeRenderer = program->nativeRenderer();

    // Main loop
    while(program->neededQuit() == false) {
        // Update delta

        SDL_Event event;
        while(SDL_PollEvent(&event) != 0) {
            program->handleEvent(&event);
        }

        // Update
        SDL_SetRenderDrawColor(nativeRenderer, 255, 255, 255, 255);
        renderer->clear();
        
        // Draw background first
        renderer->disableBlending();
        renderer->drawTGA(bg, 0, 0);
        // Before read pixels from render target, render present
        renderer->flush();
        
        // Draw the instance next
        // TODO: Draw with alpha blending
        renderer->enableBlending(SDL_BLENDMODE_BLEND);
        renderer->drawTGA(particle, 0, 0);
        renderer->flush();

        // TODO: Draw with additive blending
        renderer->enableBlending(SDL_BLENDMODE_ADD);
        renderer->drawTGA(particle, 256, 0);
        renderer->flush();

        // TODO: Draw with multiply blending
        renderer->enableBlending(SDL_BLENDMODE_MUL);
        renderer->drawTGA(particle, 512, 0);
        renderer->flush();

        // Render
        program->renderer()->present();
    }

    return 0;
}
