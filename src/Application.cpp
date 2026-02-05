#include "Application.h"

Application::Application(SDL_InitFlags flags) {
    SDL_Init(flags);
}

Application::~Application() {
    SDL_Quit();
}
