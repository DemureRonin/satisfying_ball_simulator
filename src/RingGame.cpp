#include "RingGame.h"

RingGame::RingGame(const RingGameParams &params)
    : ball_(params.ballCenter, params.ballRadius), params_(params) {
    for (int i = 0; i < params_.ringCount; ++i) {
        float outer = params_.maxRadius - i * params_.interval;
        float inner = outer - params_.ringThickness;

        RingParams rp{};
        rp.center = params_.center;
        rp.outerRadius = rp.targetOuterRadius = outer;
        rp.innerRadius = rp.targetInnerRadius = inner;

        rp.interval = params_.interval;
        rp.gapAngle = params_.gapAngle;
        rp.moveSpeed = params_.moveSpeed;

        rp.angle = 0.0f;
        rp.angularSpeed = params_.baseRotationSpeed + i * params_.rotationStep;
        rp.targetAngularSpeed = rp.angularSpeed;

        rp.rotationOffset = params_.rotationOffset * i;


        rings_.emplace_back(rp);
    }

    ball_.ApplyImpulse(params_.ballImpulse);
}

void RingGame::Simulate(SDL_Renderer *renderer, double deltaTime) {
    for (auto &r: rings_)
        r.Simulate(renderer, deltaTime);

    for (auto &p: particles_)
        p.Simulate(renderer, deltaTime);

    if (rings_.empty())
        return;

    auto &last = rings_.back();

    int out = ball_.Simulate(
        deltaTime,
        last.GetCenter(),
        last.GetInnerRadius(),
        last.GetRotationAngle(),
        last.GetRotationAngle() + last.GetGapAngleEnd()
    );

    ball_.Draw(renderer);

    if (!out)
        return;


    float placementRaduis = last.GetInnerRadius();
    SDL_Color explodeColor = last.GetColor();
    SDL_FPoint center = last.GetCenter();

    rings_.pop_back();

    particles_.emplace_front(
        params_.particlesCount,
        center,
        placementRaduis,
        params_.particleRadius,
        explodeColor
    );

    if (particles_.size() > params_.maxParticleSystems)
        particles_.pop_back();

    if (params_.moveToNextInterval) {
        for (auto &r: rings_)
            r.MoveToNextInterval();

        if (params_.addNewRing)
            AddNewRing();

        ReassignSpeeds();
    }
}

void RingGame::ReassignSpeeds() {
    for (size_t i = 0; i < rings_.size(); ++i) {
        float speed = params_.baseRotationSpeed + i * params_.rotationStep;
        rings_[i].SetTargetAngularSpeed(speed);
    }
}

void RingGame::AddNewRing() {
    float outer = params_.maxRadius;
    float inner = outer - params_.ringThickness;

    float angle = 0.0f;
    float speed = params_.baseRotationSpeed;

    if (!rings_.empty()) {
        angle = rings_.front().GetRotationAngle();
        speed = rings_.front().GetAngularSpeed();
    }

    RingParams rp{};
    rp.center = params_.center;
    rp.outerRadius = rp.targetOuterRadius = outer;
    rp.innerRadius = rp.targetInnerRadius = inner;

    rp.interval = params_.interval;
    rp.gapAngle = params_.gapAngle;
    rp.moveSpeed = params_.moveSpeed;

    rp.angle = angle;
    rp.angularSpeed = speed;
    rp.targetAngularSpeed = speed;
    rp.rotationOffset = -params_.rotationOffset;

    rings_.emplace_front(rp);
}
