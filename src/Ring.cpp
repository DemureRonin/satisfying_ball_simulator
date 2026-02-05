#include "Ring.h"

#include <algorithm>
#include <ostream>
#include <cmath>

#include "SDL3_gfx/SDL3_gfxPrimitives.h"


Ring::Ring(const RingParams &params)
    : params_(params) {
}

void Ring::Simulate(SDL_Renderer *renderer, double deltaTime) {
    float ds = params_.targetAngularSpeed - params_.angularSpeed;
    params_.angularSpeed += ds * std::min(1.0, deltaTime * 5.0f);

    params_.angle += params_.angularSpeed * deltaTime;
    if (params_.angle >= 360.0f)
        params_.angle = std::fmod(params_.angle, 360.0f);

    UpdatePosition(deltaTime);
    DrawRing(renderer);
    DrawHole(renderer);
}

void Ring::MoveToNextInterval() {
    params_.targetOuterRadius -= params_.interval;
    params_.targetInnerRadius -= params_.interval;
}

void Ring::UpdatePosition(double deltaTime) {
    auto step = [&](double c, double t) {
        double d = t - c;
        if (std::abs(d) < 0.01f) return t;
        return c + d * std::min(1.0, params_.moveSpeed * deltaTime / std::abs(d));
    };

    params_.outerRadius = step(params_.outerRadius, params_.targetOuterRadius);
    params_.innerRadius = step(params_.innerRadius, params_.targetInnerRadius);
}

float Ring::GetRotationAngle() const {
    return params_.angle + params_.rotationOffset;
}

float Ring::GetAngularSpeed() const {
    return params_.angularSpeed;
}

void Ring::SetTargetAngularSpeed(float speed) {
    params_.targetAngularSpeed = speed;
}

SDL_FPoint Ring::GetCenter() const {
    return params_.center;
}

float Ring::GetInnerRadius() const {
    return params_.innerRadius;
}

float Ring::GetGapAngleEnd() const {
    return params_.gapAngle;
}

SDL_Color Ring::GetColor() const {
    Uint8 v = static_cast<Uint8>(std::fmod(params_.innerRadius * 255 / 550, 255));
    return SDL_Color{static_cast<Uint8>(220 - v / 2), static_cast<Uint8>(200 - v), v, 255};
}

void Ring::DrawRing(SDL_Renderer *r) const {
    auto c = GetColor();
    hollowCircle(
        r,
        params_.center.x,
        params_.center.y,
        params_.outerRadius,
        params_.outerRadius - params_.innerRadius,
        params_.outerRadius,
        c.r, c.g, c.b, c.a
    );
}

void Ring::DrawHole(SDL_Renderer *r) const {
    float a = GetRotationAngle();
    aaFilledPieOuterRGBA(
        r,
        params_.center.x,
        params_.center.y,
        params_.outerRadius + 2,
        params_.outerRadius - params_.innerRadius + 4,
        a,
        a + params_.gapAngle,
        0, 0, 0, 255
    );
}


bool Ring::IsPointInHole(int px, int py,
                         int cx, int cy,
                         int startAngle, int endAngle) {
    float dx = px - cx;
    float dy = py - cy;

    if (dx == 0.0f && dy == 0.0f)
        return true;

    double angle = std::atan2(dy, dx) * 180.0 / M_PI;
    if (angle < 0.0)
        angle += 360.0;

    startAngle %= 360;
    endAngle %= 360;
    if (startAngle < 0) startAngle += 360;
    if (endAngle < 0) endAngle += 360;

    if (startAngle <= endAngle)
        return angle >= startAngle && angle <= endAngle;
    else
        return angle >= startAngle || angle <= endAngle;
}
