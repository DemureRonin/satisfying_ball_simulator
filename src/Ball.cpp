#include "Ball.h"

#include "Ring.h"

void Ball::Draw(SDL_Renderer *renderer) const {
    filledEllipseRGBA(renderer, coord_.x, coord_.y,
                      radius_, radius_, 152, 63, 137, 255);
}

bool Ball::PointInPolygon(float px, float py, const float *vx, const float *vy, int n) {
    bool inside = false;
    for (int i = 0, j = n - 1; i < n; j = i++) {
        if (((vy[i] > py) != (vy[j] > py)) &&
            (px < (vx[j] - vx[i]) * (py - vy[i]) / (vy[j] - vy[i]) + vx[i]))
            inside = !inside;
    }
    return inside;
}

int Ball::Simulate(float dt, SDL_FPoint center, float inner_radius, int startAngle, int endAngle) {
    speed_.y += g * dt;

    coord_.x += speed_.x * dt;
    coord_.y += speed_.y * dt;

    float dx = coord_.x - center.x;
    float dy = coord_.y - center.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    float limit = inner_radius - radius_;
    auto in_pol = Ring::IsPointInHole(coord_.x, coord_.y, center.x, center.y, startAngle, endAngle);

    if (dist >= inner_radius - radius_ && in_pol) {
        return 1;
    }

    if (dist >= limit && !in_pol) {
        float nx = dx / dist;
        float ny = dy / dist;

        float tx = -ny;
        float ty = nx;

        float vn = speed_.x * nx + speed_.y * ny;
        float vt = speed_.x * tx + speed_.y * ty;

        vn = -vn * 1;
        vt *= 1;

        speed_.x = vn * nx + vt * tx;
        speed_.y = vn * ny + vt * ty;

        coord_.x = center.x + nx * limit;
        coord_.y = center.y + ny * limit;
    }
    return 0;
}

void Ball::ApplyImpulse(SDL_FPoint vector) {
    speed_.x += vector.x;
    speed_.y += vector.y;
}
