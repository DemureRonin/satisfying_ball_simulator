#pragma once
#include <SDL3/SDL.h>


struct RingParams {
    SDL_FPoint center;

    float outerRadius;
    float innerRadius;
    float targetOuterRadius;
    float targetInnerRadius;

    float gapAngle;
    float interval;

    float angle;
    float angularSpeed;
    float targetAngularSpeed;
    float rotationOffset;

    float moveSpeed;
};

class Ring {
public:
    explicit Ring(const RingParams &params);

    void Simulate(SDL_Renderer *renderer, double deltaTime);

    void MoveToNextInterval();

    void SetTargetAngularSpeed(float speed);

    [[nodiscard]] float GetRotationAngle() const;

    [[nodiscard]] float GetAngularSpeed() const;

    [[nodiscard]] SDL_FPoint GetCenter() const;

    [[nodiscard]] float GetInnerRadius() const;

    [[nodiscard]] float GetGapAngleEnd() const;

    [[nodiscard]] SDL_Color GetColor() const;

    static bool IsPointInHole(int px, int py, int cx, int cy, int startAngle, int endAngle);

private:
    RingParams params_;

    void UpdatePosition(double deltaTime);

    void DrawRing(SDL_Renderer *r) const;

    void DrawHole(SDL_Renderer *r) const;
};
