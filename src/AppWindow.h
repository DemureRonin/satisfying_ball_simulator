#pragma once

#include <string>
#include <SDL3/SDL.h>


class AppWindow {
public:
    AppWindow(const std::string &title, int width, int height, SDL_WindowFlags flags);

    ~AppWindow();

    AppWindow(const AppWindow &) = delete;

    AppWindow &operator=(const AppWindow &) = delete;

    AppWindow(AppWindow &&other) noexcept;

    AppWindow &operator=(AppWindow &&other) noexcept;


    void ClearBackground(float r = 0, float g = 0, float b = 0, float a = 1.0f) const;

    void Render() const;

    static void ImGuiNewFrame();

    void ImGuiRender() const;

    [[nodiscard]] SDL_Window *GetWindow() const { return window_; }
    [[nodiscard]] SDL_Renderer *GetRenderer() const { return renderer_; }
    [[nodiscard]] bool IsValid() const { return window_ != nullptr && renderer_ != nullptr; }

private:
    SDL_Window *window_ = nullptr;
    SDL_Renderer *renderer_ = nullptr;
};
