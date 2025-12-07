#include "raylib.h"
#include "app/App.hpp"
#include <iostream>

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "NCTU SIS - Student Information System");
    SetTargetFPS(60);

    App app;
    if (!app.init()) {
        std::cerr << "Failed to initialize application" << std::endl;
        CloseWindow();
        return 1;
    }

    while (!WindowShouldClose() && !app.shouldQuit) {
        float dt = GetFrameTime();

        app.handleEvent();
        app.update(dt);

        BeginDrawing();
        ClearBackground((Color){20, 25, 40, 255}); // Dark background

        app.draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

