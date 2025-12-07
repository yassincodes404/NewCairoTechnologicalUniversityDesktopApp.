#include "CourseListScreen.hpp"
#include "../ScreenType.hpp"
#include <sstream>
#include <iomanip>

CourseListScreen::CourseListScreen(App& app)
    : app(app),
      courseTable(250, 180, 35),
      backButton(50, 50, 100, 40, "Back"),
      searchInput(250.0f, 130.0f, 400.0f, 30.0f),
      selectedIndex(-1) {
    
    // Fetch courses
    allCourses = app.courseService.listCourses();
    courses = allCourses;
    
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
    searchInput.handleEvent();

    // Case-insensitive filtering of courses
    auto toLower = [](const std::string& s) {
        std::string out = s;
        for (char& ch : out) {
            if (ch >= 'A' && ch <= 'Z') {
                ch = (char)(ch - 'A' + 'a');
            }
        }
        return out;
    };

    std::string query = searchInput.getText();
    std::string queryLower = toLower(query);

    bool queryChanged = (queryLower != lastQuery);

    if (queryChanged) {
        lastQuery = queryLower;
        selectedIndex = -1;

        courses.clear();
        courseTable.clear();
        courseTable.setHeaders({"Course Code", "Title", "Credits"});
        courseTable.setColumnWidths({150, 400, 100});

        if (!queryLower.empty()) {
            for (const auto& c : allCourses) {
                std::string line = c.courseCode + " " + c.title;
                if (toLower(line).find(queryLower) != std::string::npos) {
                    courses.push_back(c);
                }
            }
        } else {
            courses = allCourses;
        }

        for (const auto& course : courses) {
            std::vector<std::string> row = {
                course.courseCode,
                course.title,
                std::to_string(course.credits)
            };
            courseTable.addRow(row);
        }
    }

    // Let the table handle scrolling and keyboard navigation
    courseTable.update();
    
    if (backButton.isClicked()) {
        app.switchScreen(ScreenType::MainMenu);
        return;
    }
    
    // Selection from table (mouse or keyboard inside SimpleTable)
    int tableSelected = courseTable.getSelectedRow();
    if (tableSelected >= 0 && tableSelected < (int)courses.size()) {
        selectedIndex = tableSelected;
    }
}

void CourseListScreen::update(float dt) {
    // Update animations if needed
}

void CourseListScreen::draw() {
    backButton.draw();
    
    const char* title = "Courses";
    DrawTextEx(GetFontDefault(), title, {250, 80}, 32, 2, WHITE);

    DrawText("Search:", 250, 120, 16, LIGHTGRAY);
    searchInput.draw();
    
    if (courses.empty()) {
        DrawText("No courses found", 250, 200, 20, GRAY);
        return;
    }
    
    // Draw course count
    std::stringstream ss;
    ss << "Total courses: " << courses.size();
    DrawText(ss.str().c_str(), 250, 160, 18, LIGHTGRAY);
    
    // Draw table
    courseTable.draw();
    
    // Instructions
    DrawText("Use arrow keys to navigate or click on a course", 
             250, (float)GetScreenHeight() - 50, 16, GRAY);
}

