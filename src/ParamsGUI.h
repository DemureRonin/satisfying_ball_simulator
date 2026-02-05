#pragma once
#include "imgui/imgui.h"
#include "RingGame.h"

inline void ShowParamsGUI(RingGameParams &params, bool &shouldRestart) {
    ImGui::Begin("Game Parameters", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    bool interacted = false;

    ImGui::SeparatorText("Rotation Center");
    interacted |= ImGui::DragFloat2("Center", &params.center.x, 1.0f, 0.0f, 1000.0f, "%.0f");

    ImGui::SeparatorText("Rings Configuration");
    interacted |= ImGui::SliderInt("Ring Count", &params.ringCount, 1, 100);
    interacted |= ImGui::DragFloat("Max Radius", &params.maxRadius, 1.0f, 10.0f, 1000.0f, "%.0f");
    interacted |= ImGui::DragFloat("Ring Thickness", &params.ringThickness, 0.1f, 1.0f, 50.0f, "%.1f");
    interacted |= ImGui::DragFloat("Interval", &params.interval, 0.1f, 1.0f, 100.0f, "%.1f");

    ImGui::SeparatorText("Rotation");
    interacted |= ImGui::DragFloat("Base Speed", &params.baseRotationSpeed, 1.0f, -360.0f, 360.0f, "%.0f deg/s");
    interacted |= ImGui::DragFloat("Speed Step", &params.rotationStep, 0.1f, -10.0f, 10.0f, "%.1f");
    interacted |= ImGui::DragFloat("Rotation Offset", &params.rotationOffset, 0.1f, 0.0f, 360.0f, "%.1f deg");

    ImGui::SeparatorText("Movement");
    interacted |= ImGui::DragFloat("Move Speed", &params.moveSpeed, 1.0f, 0.0f, 1000.0f, "%.0f");
    interacted |= ImGui::DragFloat("Gap Angle", &params.gapAngle, 0.5f, 0.0f, 360.0f, "%.1f deg");

    ImGui::SeparatorText("Ball");
    interacted |= ImGui::DragFloat2("Ball Center", &params.ballCenter.x, 1.0f, 0.0f, 1000.0f, "%.0f");
    interacted |= ImGui::DragFloat2("Ball Impulse", &params.ballImpulse.x, 1.0f, -500.0f, 500.0f, "%.0f");
    interacted |= ImGui::DragFloat("Ball Radius", &params.ballRadius, 0.1f, 1.0f, 50.0f, "%.1f");

    ImGui::SeparatorText("Particles");
    interacted |= ImGui::SliderInt("Particles Count", &params.particlesCount, 1, 500);
    interacted |= ImGui::SliderInt("Max Systems", &params.maxParticleSystems, 1, 100);
    interacted |= ImGui::DragFloat("Particle Radius", &params.particleRadius, 0.1f, 0.1f, 20.0f, "%.1f");

    ImGui::SeparatorText("Flags");
    interacted |= ImGui::Checkbox("Move to Next Interval", &params.moveToNextInterval);
    interacted |= ImGui::Checkbox("Add New Ring", &params.addNewRing);


    if (ImGui::Button("Reset to Default")) {
        params = RingGameParams();
        shouldRestart = true;
    }

    ImGui::SameLine();

    if (ImGui::Button("Restart Game")) {
        shouldRestart = true;
    }

    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Reset all parameters to default values");
    }

    if (ImGui::CollapsingHeader("Current Values")) {
        ImGui::Text("Rings: %d", params.ringCount);
        ImGui::Text("Rotation: %.0f deg/s", params.baseRotationSpeed);
        ImGui::Text("Ball: (%.0f, %.0f)", params.ballCenter.x, params.ballCenter.y);
        ImGui::Text("Particles: %d", params.particlesCount);
    }

    if (interacted) {
        shouldRestart = true;
    }

    ImGui::End();
}
