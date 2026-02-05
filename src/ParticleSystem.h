#pragma once
#include <algorithm>
#include <vector>

#include "SDL3/SDL_rect.h"
#include "SDL3_gfx/SDL3_gfxPrimitives.h"

class ParticleSystem {
public:
    ParticleSystem(int amount, SDL_FPoint center, float placementRaduis, float particleRadius, SDL_Color color) : particleRadius_(particleRadius), color_(color) {
        points_.reserve(amount);
        PointsOnCircle(center.x, center.y, placementRaduis, amount);
    }


    void Simulate(SDL_Renderer *renderer, float deltaTime) {
        for (auto &point: points_) {
            point.y += particleSpeed_ * deltaTime;


            filledEllipseRGBA(renderer, point.x, point.y,
                              particleRadius_, particleRadius_, color_.r, color_.g, color_.b, 255 - alpha);
            alpha += deltaTime * fadeSpeed_;
            alpha = std::clamp(alpha, 0.f, 255.f);
        }
    }

    void PointsOnCircle(float cx, float cy, float radius, int count) {
        const float step = 2.0f * M_PI / count;

        for (int i = 0; i < count; ++i) {
            float a = step * i;
            points_.push_back({
                cx + radius * std::cos(a),
                cy + radius * std::sin(a)
            });
        }
    }

private:
    std::vector<SDL_FPoint> points_;
    float particleRadius_;
    float particleSpeed_ = 500;
    float fadeSpeed_ = 10;
    float alpha = 0;
    SDL_Color color_;
};
