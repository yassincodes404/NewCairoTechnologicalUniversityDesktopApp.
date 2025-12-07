#include "CourseListScreen.hpp"
#include "../ScreenType.hpp"
#include <sstream>
#include <iomanip>

CourseListScreen::CourseListScreen(App& app)
    : app(app),
      courseTable(250, 150, 35),
      backButton(50, 50, 100, 40, "Back"),
      selectedIndex(-1) {
    
    // Fetch courses
    courses = app.courseService.listCourses();
    
    // Setup table
    courseTable.setHeaders({"Course Code", "Title", "Credits"});
    courseTable.setColumnWidths({150, 400, 100});
    
    // Add rows
    for (const auto& course : courses) {
        std::vector<std::string> row = {
            course.courseCode,
            course.title,
            std::to_string(course.credits)
        };
        courseTable.addRow(row);
    }
}

void CourseListScreen::handleEvent() {
    backButton.update();
    courseTable.update();
    
    if (backButton.isClicked()) {
        app.switchScreen(ScreenType::MainMenu);
        return;
    }
    
    // Keyboard navigation
    if (IsKeyPressed(KEY_UP) && selectedIndex > 0) {
        selectedIndex--;
    }
    if (IsKeyPressed(KEY_DOWN) && selectedIndex < (int)courses.size() - 1) {
        selectedIndex++;
    }
    
    // Mouse selection
    int tableSelected = courseTable.getSelectedRow();
    if (tableSelected >= 0) {
        selectedIndex = tableSelected;
    }
}

void CourseListScreen::update(float dt) {
    // Update animations if needed
}

void CourseListScreen::draw() {
    backButton.draw();
    
    const char* title = "Courses";
    DrawTextEx(GetFontDefault(), title, {250, 100}, 32, 2, WHITE);
    
    if (courses.empty()) {
        DrawText("No courses found", 250, 200, 20, GRAY);
        return;
    }
    
    // Draw course count
    std::stringstream ss;
    ss << "Total courses: " << courses.size();
    DrawText(ss.str().c_str(), 250, 120, 18, LIGHTGRAY);
    
    // Draw table
    courseTable.draw();
    
    // Instructions
    DrawText("Use arrow keys to navigate or click on a course", 
             250, (float)GetScreenHeight() - 50, 16, GRAY);
}

