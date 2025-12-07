#ifndef COURSE_LIST_SCREEN_HPP
#define COURSE_LIST_SCREEN_HPP

#include "../Screen.hpp"
#include "../widgets/Button.hpp"
#include "../widgets/SimpleTable.hpp"
#include "../widgets/TextInput.hpp"
#include "../../app/App.hpp"
#include "../../core/models/Course.hpp"
#include <vector>

class CourseListScreen : public Screen {
public:
    explicit CourseListScreen(App& app);
    void handleEvent() override;
    void update(float dt) override;
    void draw() override;

private:
    App& app;
    std::vector<Course> allCourses;
    std::vector<Course> courses;
    SimpleTable courseTable;
    Button backButton;
    TextInput searchInput;
    std::string lastQuery;
    int selectedIndex;
};

#endif // COURSE_LIST_SCREEN_HPP

