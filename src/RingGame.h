#pragma once
#include <deque>
#include "Ball.h"
#include "ParticleSystem.h"
#include "Ring.h"

struct RingGameParams {
    SDL_FPoint center{500, 500};
    int ringCount = 24;

    float maxRadius = 300;
    float ringThickness = 4;
    float interval = 10;

    float baseRotationSpeed = 120;
    float rotationStep = 0;
    float rotationOffset = 10;

    float moveSpeed = 150;
    float gapAngle = 70;

    SDL_FPoint ballCenter{500, 530};
    SDL_FPoint ballImpulse{300, 250};
    float ballRadius = 6;
    bool moveToNextInterval = true;
    bool addNewRing = true;

    int particlesCount = 50;
    int maxParticleSystems = 20;
    float particleRadius = 3;
};

class RingGame {
public:
    explicit RingGame(const RingGameParams &params);

    void Simulate(SDL_Renderer *renderer, double deltaTime);

private:
    std::deque<Ring> rings_;
    Ball ball_;
    RingGameParams params_;
    std::deque<ParticleSystem> particles_;

    void ReassignSpeeds();

    void AddNewRing();
};
