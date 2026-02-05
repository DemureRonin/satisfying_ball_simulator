#pragma once
#include "imgui/backends/imgui_impl_sdl3.h"

class EventHandler {
public:
    static void PollEvents(bool &done, bool &shouldRestart) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT)
                done = true;
            if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_ESCAPE)
                done = true;
            if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_Q)
                done = true;
            if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_R)
                shouldRestart = true;

            ImGui_ImplSDL3_ProcessEvent(&e);
        }
    }
};
