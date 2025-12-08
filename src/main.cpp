#include "raylib.h"
#include "app/App.hpp"
#include <cstdlib>
#include <exception>
#include <iostream>

namespace {
struct WindowConfig {
    int width;
    int height;
    const char* title;
    int targetFps;
    Color clearColor;
};

class WindowGuard {
public:
    explicit WindowGuard(const WindowConfig& config)
        : initialized(false) {
        InitWindow(config.width, config.height, config.title);
        if (IsWindowReady()) {
            SetTargetFPS(config.targetFps);
            clearColor = config.clearColor;
            initialized = true;
        }
    }

    WindowGuard(const WindowGuard&) = delete;
    WindowGuard& operator=(const WindowGuard&) = delete;

    ~WindowGuard() {
        if (initialized && IsWindowReady()) {
            CloseWindow();
        }
    }

    bool isInitialized() const {
        return initialized;
    }

    Color getClearColor() const {
        return clearColor;
    }

private:
    bool initialized;
    Color clearColor;
};
} // namespace

int main() {
    const WindowConfig config{
        1280,
        720,
        "NCTU SIS - Student Information System",
        60,
        (Color){20, 25, 40, 255} // Dark background
    };

    try {
        WindowGuard window(config);
        if (!window.isInitialized()) {
            std::cerr << "Failed to create window" << std::endl;
            return EXIT_FAILURE;
        }

        App app;
        if (!app.init()) {
            std::cerr << "Failed to initialize application" << std::endl;
            return EXIT_FAILURE;
        }

        while (!WindowShouldClose() && !app.shouldQuit) {
            const float dt = GetFrameTime();

            app.handleEvent();
            app.update(dt);

            BeginDrawing();
            ClearBackground(window.getClearColor());
            app.draw();
            EndDrawing();
        }

        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "Unhandled std::exception in main: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unhandled unknown exception in main." << std::endl;
    }

    return EXIT_FAILURE;
}

