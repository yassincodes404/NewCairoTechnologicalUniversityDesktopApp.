#include "MainMenuScreen.hpp"
#include "../ScreenType.hpp"

MainMenuScreen::MainMenuScreen(App& app)
    : app(app),
      studentsButton(400, 230, 300, 50, "Students"),
      programsButton(400, 300, 300, 50, "Programs & Curricula"),
      coursesButton(400, 370, 300, 50, "Courses"),
      quitButton(400, 440, 300, 50, "Quit") {
}

void MainMenuScreen::handleEvent() {
    studentsButton.update();
    programsButton.update();
    coursesButton.update();
    quitButton.update();

    if (studentsButton.isClicked()) {
        app.switchScreen(ScreenType::StudentList);
    }

    if (programsButton.isClicked()) {
        app.switchScreen(ScreenType::ProgramView);
    }

    if (coursesButton.isClicked()) {
        app.switchScreen(ScreenType::CourseList);
    }

    if (quitButton.isClicked() || IsKeyPressed(KEY_ESCAPE)) {
        app.shouldQuit = true;
    }
}

void MainMenuScreen::update(float dt) {
    // Update animations if needed
}

void MainMenuScreen::draw() {
    const char* title = "Main Menu";
    Vector2 titleSize = MeasureTextEx(GetFontDefault(), title, 36, 2);
    DrawTextEx(GetFontDefault(), title,
               {(float)GetScreenWidth() / 2 - titleSize.x / 2, 150},
               36, 2, WHITE);

    studentsButton.draw();
    programsButton.draw();
    coursesButton.draw();
    quitButton.draw();
}

