#pragma once
#include "SDL3_gfx/SDL3_gfxPrimitives.h"

class Ball {
public:
    Ball(SDL_FPoint coord, float radius) : coord_(coord), radius_(radius) {
    }

    void Draw(SDL_Renderer *renderer) const;

    void ApplyImpulse(SDL_FPoint vector);

    int Simulate(float dt, SDL_FPoint center, float inner_radius, int startAngle, int endAngle);

    static bool PointInPolygon(float px, float py, const float *vx, const float *vy, int n);

    [[nodiscard]] SDL_FPoint GetPosition() const { return coord_; }

    [[nodiscard]] SDL_FPoint GetSpeed() const { return speed_; }

private:
    static constexpr float g = 400;
    SDL_FPoint coord_;
    SDL_FPoint speed_{0, 0};
    float radius_;
};
