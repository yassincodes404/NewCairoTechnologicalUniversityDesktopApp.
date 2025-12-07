#include "StudentListScreen.hpp"
#include "../ScreenType.hpp"

StudentListScreen::StudentListScreen(App& app)
    : app(app),
      allStudents(app.studentService.listStudents()),
      students(allStudents),
      selectedIndex(0),
      firstVisibleIndex(0),
      backButton(50, 50, 100, 40, "Back"),
      searchInput(250.0f, 110.0f, 400.0f, 30.0f),
      scrollY(0.0f) {
}

void StudentListScreen::handleEvent() {
    backButton.update();
    searchInput.handleEvent();

    if (backButton.isClicked()) {
        app.switchScreen(ScreenType::MainMenu);
        return;
    }

    // Filter students by search text (case-insensitive substring)
    static std::string lastQuery;
    std::string query = searchInput.getText();

    auto toLower = [](const std::string& s) {
        std::string out = s;
        for (char& ch : out) {
            if (ch >= 'A' && ch <= 'Z') {
                ch = (char)(ch - 'A' + 'a');
            }
        }
        return out;
    };

    std::string queryLower = toLower(query);

    if (queryLower != lastQuery) {
        // Reset selection and scroll when query changes
        firstVisibleIndex = 0;
        selectedIndex = 0;
        lastQuery = queryLower;
    }

    if (!queryLower.empty()) {
        std::vector<Student> filtered;
        for (const auto& s : allStudents) {
            std::string line = s.studentCode + " " + s.firstName + " " + s.lastName + " " + s.program;
            if (toLower(line).find(queryLower) != std::string::npos) {
                filtered.push_back(s);
            }
        }
        students = filtered;
    } else {
        students = allStudents;
    }

    if (students.empty()) {
        selectedIndex = -1;
        firstVisibleIndex = 0;
    } else {
        if (selectedIndex < 0) selectedIndex = 0;
        if (selectedIndex >= (int)students.size()) {
            selectedIndex = (int)students.size() - 1;
        }
        if (firstVisibleIndex >= (int)students.size()) {
            firstVisibleIndex = 0;
        }
    }

    // Keyboard navigation with vertical scrolling
    float startY = 150.0f;
    float rowHeight = 40.0f;
    int maxVisible = (int)((GetScreenHeight() - startY - 80.0f) / rowHeight);
    if (maxVisible < 1) maxVisible = 1;

    if (IsKeyPressed(KEY_UP) && selectedIndex > 0) {
        selectedIndex--;
        if (selectedIndex < firstVisibleIndex) {
            firstVisibleIndex = selectedIndex;
        }
    }
    if (IsKeyPressed(KEY_DOWN) && selectedIndex < (int)students.size() - 1) {
        selectedIndex++;
        if (selectedIndex >= firstVisibleIndex + maxVisible) {
            firstVisibleIndex = selectedIndex - maxVisible + 1;
        }
    }

    // Select with Enter
    if (IsKeyPressed(KEY_ENTER) && !students.empty()) {
        app.selectedStudent = students[selectedIndex];
        app.switchScreen(ScreenType::StudentDetails);
    }

    // Mouse selection on visible rows only
    Vector2 mousePos = GetMousePosition();
    float startYMouse = 150.0f;
    float rowHeightMouse = 40.0f;
    int maxVisibleMouse = (int)((GetScreenHeight() - startYMouse - 80.0f) / rowHeightMouse);
    if (maxVisibleMouse < 1) maxVisibleMouse = 1;

    int endIndexMouse = firstVisibleIndex + maxVisibleMouse;
    if (endIndexMouse > (int)students.size()) endIndexMouse = (int)students.size();

    for (int i = firstVisibleIndex; i < endIndexMouse; ++i) {
        float y = startYMouse + (i - firstVisibleIndex) * rowHeightMouse;
        Rectangle rowRect = {250.0f, y, 600.0f, rowHeightMouse};
        if (CheckCollisionPointRec(mousePos, rowRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            selectedIndex = i;
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
    DrawTextEx(GetFontDefault(), title, {250, 70}, 32, 2, WHITE);

    // Search label and box
    DrawText("Search:", 250, 105, 16, LIGHTGRAY);
    searchInput.draw();

    if (students.empty()) {
        DrawText("No students found", 250, 200, 20, GRAY);
        return;
    }

    float startY = 160.0f;
    float rowHeight = 40.0f;
    int maxVisible = (int)((GetScreenHeight() - startY - 80.0f) / rowHeight);
    if (maxVisible < 1) maxVisible = 1;

    int endIndex = firstVisibleIndex + maxVisible;
    if (endIndex > (int)students.size()) endIndex = (int)students.size();

    // Left: list of students
    for (int i = firstVisibleIndex; i < endIndex; ++i) {
        const auto& student = students[i];
        float y = startY + (i - firstVisibleIndex) * rowHeight;

        // Highlight selected row
        if (i == selectedIndex) {
            DrawRectangle(250, (int)y, 600, (int)rowHeight, (Color){60, 80, 120, 255});
        }

        // Draw student basic line
        std::string line = student.studentCode + " - " + student.firstName + " " +
                           student.lastName + " - " + student.program + " (Level " +
                           std::to_string(student.level) + ")";
        DrawText(line.c_str(), 260, (int)y + 10, 18, WHITE);
    }

    // Right: selected student summary card with profile picture
    if (selectedIndex >= 0 && selectedIndex < (int)students.size()) {
        const auto& s = students[selectedIndex];

        float cardX = 900.0f;
        float cardY = 150.0f;
        float cardW = 320.0f;
        float cardH = 220.0f;

        // Card background
        DrawRectangle((int)cardX, (int)cardY, (int)cardW, (int)cardH, (Color){30, 30, 45, 255});
        DrawRectangleLines((int)cardX, (int)cardY, (int)cardW, (int)cardH, (Color){100, 150, 255, 255});

        // Avatar
        float avatarX = cardX + 50.0f;
        float avatarY = cardY + 60.0f;
        float avatarRadius = 30.0f;
        DrawCircle((int)avatarX, (int)avatarY, avatarRadius, (Color){60, 80, 120, 255});
        DrawCircleLines((int)avatarX, (int)avatarY, avatarRadius, (Color){150, 180, 255, 255});

        std::string initials;
        if (!s.firstName.empty()) initials += (char)std::toupper(s.firstName[0]);
        if (!s.lastName.empty()) initials += (char)std::toupper(s.lastName[0]);
        if (!initials.empty()) {
            int fontSize = 24;
            int textWidth = MeasureText(initials.c_str(), fontSize);
            DrawText(initials.c_str(),
                     (int)(avatarX - textWidth / 2),
                     (int)(avatarY - fontSize / 2),
                     fontSize,
                     WHITE);
        }

        // Info text
        float infoX = cardX + 100.0f;
        float infoY = cardY + 30.0f;
        DrawText(("Code: " + s.studentCode).c_str(), (int)infoX, (int)infoY, 18, WHITE);
        infoY += 24;
        std::string fullName = s.firstName + " " + s.lastName;
        DrawText(fullName.c_str(), (int)infoX, (int)infoY, 18, LIGHTGRAY);
        infoY += 24;
        DrawText(("Program: " + s.program).c_str(), (int)infoX, (int)infoY, 18, LIGHTGRAY);
        infoY += 24;
        DrawText(("Level: " + std::to_string(s.level)).c_str(), (int)infoX, (int)infoY, 18, LIGHTGRAY);
        infoY += 24;
        if (!s.academicStatus.empty()) {
            DrawText(("Status: " + s.academicStatus).c_str(), (int)infoX, (int)infoY, 18, LIGHTGRAY);
        }
    }

    // Instructions
    DrawText("Use arrow keys to navigate, Enter to select, or click on a student", 
             250, (float)GetScreenHeight() - 50, 16, GRAY);
}

