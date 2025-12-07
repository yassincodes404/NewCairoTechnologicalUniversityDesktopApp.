#include "StudentListScreen.hpp"
#include "../ScreenType.hpp"

StudentListScreen::StudentListScreen(App& app)
    : app(app),
      students(app.studentService.listStudents()),
      selectedIndex(0),
      backButton(50, 50, 100, 40, "Back"),
      scrollY(0.0f) {
}

void StudentListScreen::handleEvent() {
    backButton.update();

    if (backButton.isClicked()) {
        app.switchScreen(ScreenType::MainMenu);
        return;
    }

    // Keyboard navigation
    if (IsKeyPressed(KEY_UP) && selectedIndex > 0) {
        selectedIndex--;
    }
    if (IsKeyPressed(KEY_DOWN) && selectedIndex < (int)students.size() - 1) {
        selectedIndex++;
    }

    // Select with Enter
    if (IsKeyPressed(KEY_ENTER) && !students.empty()) {
        app.selectedStudent = students[selectedIndex];
        app.switchScreen(ScreenType::StudentDetails);
    }

    // Mouse selection
    Vector2 mousePos = GetMousePosition();
    float startY = 150.0f;
    float rowHeight = 40.0f;

    for (size_t i = 0; i < students.size(); i++) {
        Rectangle rowRect = {250.0f, startY + i * rowHeight, 600.0f, rowHeight};
        if (CheckCollisionPointRec(mousePos, rowRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            selectedIndex = (int)i;
            app.selectedStudent = students[i];
            app.switchScreen(ScreenType::StudentDetails);
            break;
        }
    }
}

void StudentListScreen::update(float dt) {
    // Update animations if needed
}

void StudentListScreen::draw() {
    backButton.draw();

    const char* title = "Students";
    DrawTextEx(GetFontDefault(), title, {250, 100}, 32, 2, WHITE);

    if (students.empty()) {
        DrawText("No students found", 250, 200, 20, GRAY);
        return;
    }

    float startY = 150.0f;
    float rowHeight = 40.0f;

    for (size_t i = 0; i < students.size(); i++) {
        const auto& student = students[i];
        float y = startY + i * rowHeight;

        // Highlight selected row
        if ((int)i == selectedIndex) {
            DrawRectangle(250, (int)y, 600, (int)rowHeight, (Color){60, 80, 120, 255});
        }

        // Draw student info
        std::string line = student.studentCode + " - " + student.firstName + " " + 
                          student.lastName + " - " + student.program + " (Level " + 
                          std::to_string(student.level) + ")";
        DrawText(line.c_str(), 260, (int)y + 10, 18, WHITE);
    }

    // Instructions
    DrawText("Use arrow keys to navigate, Enter to select, or click on a student", 
             250, (float)GetScreenHeight() - 50, 16, GRAY);
}

