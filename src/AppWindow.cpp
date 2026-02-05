#include "AppWindow.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl3.h"
#include "imgui/backends/imgui_impl_sdlrenderer3.h"

AppWindow::AppWindow(const std::string &title, int width, int height, SDL_WindowFlags flags) {
    window_ = SDL_CreateWindow(title.c_str(), width, height, flags);
    if (!window_) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s", SDL_GetError());
        return;
    }

    renderer_ = SDL_CreateRenderer(window_, nullptr);
    if (!renderer_) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create renderer: %s", SDL_GetError());
        return;
    }
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForSDLRenderer(window_, renderer_);
    ImGui_ImplSDLRenderer3_Init(renderer_);
}

AppWindow::~AppWindow() {
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    if (renderer_) SDL_DestroyRenderer(renderer_);
    if (window_) SDL_DestroyWindow(window_);
}

AppWindow::AppWindow(AppWindow &&other) noexcept : window_(other.window_), renderer_(other.renderer_) {
    other.window_ = nullptr;
    other.renderer_ = nullptr;
}

AppWindow &AppWindow::operator=(AppWindow &&other) noexcept {
    if (this != &other) {
        if (renderer_) SDL_DestroyRenderer(renderer_);
        if (window_) SDL_DestroyWindow(window_);
        window_ = other.window_;
        renderer_ = other.renderer_;
        other.window_ = nullptr;
        other.renderer_ = nullptr;
    }
    return *this;
}

void AppWindow::ClearBackground(float r, float g, float b, float a) const {
    SDL_SetRenderDrawColorFloat(renderer_, r, g, b, a);
    SDL_RenderClear(renderer_);
}

void AppWindow::Render() const {
    SDL_RenderPresent(renderer_);
}

void AppWindow::ImGuiNewFrame() {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void AppWindow::ImGuiRender() const {
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer_);
}
