#include <memory>

#include "Application.h"
#include "AppWindow.h"
#include "EventHadler.h"
#include "ParamsGUI.h"
#include "RingGame.h"
#include "Time.h"
#include "imgui/imgui.h"


int main(int argc, char *argv[]) {
    Application app(SDL_INIT_VIDEO);
    AppWindow window("Satisfying Ball", 1920, 1080, SDL_WINDOW_OPENGL);
    if (!window.IsValid()) return 1;
    Time time;

    RingGameParams params;

    std::unique_ptr<RingGame> game = std::make_unique<RingGame>(params);

    bool done = false;
    bool shouldRestart = false;

    while (!done) {
        EventHandler::PollEvents(done, shouldRestart);

        if (shouldRestart) {
            game.reset();
            game = std::make_unique<RingGame>(params);
            shouldRestart = false;
        }

        time.Tick();

        AppWindow::ImGuiNewFrame();
        window.ClearBackground();

        game->Simulate(window.GetRenderer(), time.GetDeltaTime());

        ShowParamsGUI(params, shouldRestart);

        window.ImGuiRender();
        window.Render();
    }

    return 0;
}
