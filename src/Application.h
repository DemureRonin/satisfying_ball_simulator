#pragma once
#include "SDL3/SDL_init.h"

class Application {
public:
    explicit Application(SDL_InitFlags flags);

    int Run() {
        return 0;
    }

    ~Application();
};
