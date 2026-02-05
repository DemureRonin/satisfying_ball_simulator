#pragma once
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"

class Time {
public:
    Time() : prev_(SDL_GetPerformanceCounter()), freq_(static_cast<double>(SDL_GetPerformanceFrequency())) {
    }

    void Tick() {
        now_ = SDL_GetPerformanceCounter();
        deltaTime = static_cast<double>(now_ - prev_) / freq_;
        prev_ = now_;
    }

    [[nodiscard]] double GetDeltaTime() const {
        return deltaTime;
    }

private:
    Uint64 prev_;
    double freq_;
    Uint64 now_ = 0;
    double deltaTime = 0;
};
