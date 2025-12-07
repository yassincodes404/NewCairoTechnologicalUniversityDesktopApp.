#include "ProgramViewScreen.hpp"
#include "../ScreenType.hpp"
#include <sstream>

ProgramViewScreen::ProgramViewScreen(App& app)
    : app(app),
      selectedProgramIndex(0),
      selectedLevel(1),
      curriculumTable(300, 180, 32),
      backButton(50, 50, 100, 40, "Back"),
      levelButtons{
          Button(300, 130, 80, 35, "L1"),
          Button(390, 130, 80, 35, "L2"),
          Button(480, 130, 80, 35, "L3"),
          Button(570, 130, 80, 35, "L4")
      } {

    programs = app.programService.listPrograms();

    curriculumTable.setHeaders({"Semester", "Course Code", "Title", "Credits"});
    curriculumTable.setColumnWidths({140, 130, 360, 80});

    if (!programs.empty()) {
        loadCurriculum();
    }
}

void ProgramViewScreen::loadCurriculum() {
    curriculumTable.clear();
    currentEntries.clear();

    if (programs.empty() || selectedProgramIndex < 0 || selectedProgramIndex >= (int)programs.size()) {
        return;
    }

    int programId = programs[selectedProgramIndex].id;
    currentEntries = app.programService.getCoursesForLevel(programId, selectedLevel);

    for (const auto& entry : currentEntries) {
        std::vector<std::string> row = {
            entry.semesterLabel,
            entry.course.courseCode,
            entry.course.title,
            std::to_string(entry.course.credits)
        };
        curriculumTable.addRow(row);
    }
}

void ProgramViewScreen::handleEvent() {
    backButton.update();
    curriculumTable.update();

    if (backButton.isClicked()) {
        app.switchScreen(ScreenType::MainMenu);
        return;
    }

    // Program selection via keyboard (left/right)
    if (!programs.empty()) {
        if (IsKeyPressed(KEY_LEFT) && selectedProgramIndex > 0) {
            selectedProgramIndex--;
            loadCurriculum();
        } else if (IsKeyPressed(KEY_RIGHT) && selectedProgramIndex < (int)programs.size() - 1) {
            selectedProgramIndex++;
            loadCurriculum();
        }
    }

    // Level buttons
    for (int i = 0; i < 4; ++i) {
        levelButtons[i].update();
        if (levelButtons[i].isClicked()) {
            selectedLevel = i + 1;
            loadCurriculum();
        }
    }
}

void ProgramViewScreen::update(float dt) {
    // No animations for now
}

void ProgramViewScreen::draw() {
    backButton.draw();

    const char* title = "Programs & Curricula";
    DrawTextEx(GetFontDefault(), title, {300, 60}, 32, 2, WHITE);

    if (programs.empty()) {
        DrawText("No programs found in database.", 300, 140, 20, GRAY);
        return;
    }

    // Show selected program
    const Program& prog = programs[selectedProgramIndex];
    std::stringstream header;
    header << "Program: " << prog.name << " (" << prog.code << ")";
    DrawText(header.str().c_str(), 300, 100, 20, WHITE);

    // Draw hints for changing program
    DrawText("Use LEFT/RIGHT arrows to switch program", 300, 420, 16, GRAY);

    // Draw level buttons (use same style as generic buttons)
    for (int i = 0; i < 4; ++i) {
        // Temporarily change button color by drawing a rectangle behind it
        Color color = (i + 1 == selectedLevel) ? (Color){100, 150, 255, 255} : DARKGRAY;
        Rectangle bounds = {300.0f + i * 90.0f, 130.0f, 80.0f, 35.0f};
        DrawRectangleRec(bounds, color);
        DrawRectangleLinesEx(bounds, 1.0f, BLACK);

        std::string text = "Level " + std::to_string(i + 1);
        int textWidth = MeasureText(text.c_str(), 18);
        DrawText(text.c_str(),
                 (int)(bounds.x + (bounds.width - textWidth) / 2),
                 (int)(bounds.y + 8),
                 18,
                 WHITE);

        // Finally draw the actual button text/hover state if needed
        levelButtons[i].draw();
    }

    // Draw curriculum table
    if (currentEntries.empty()) {
        DrawText("No courses defined for this level.", 300, 220, 20, GRAY);
    } else {
        curriculumTable.draw();
    }
}