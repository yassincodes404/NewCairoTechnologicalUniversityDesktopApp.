#ifndef STUDENT_LIST_SCREEN_HPP
#define STUDENT_LIST_SCREEN_HPP

#include "../Screen.hpp"
#include "../widgets/Button.hpp"
#include "../../app/App.hpp"
#include "../../core/models/Student.hpp"
#include <vector>

class StudentListScreen : public Screen {
public:
    explicit StudentListScreen(App& app);
    void handleEvent() override;
    void update(float dt) override;
    void draw() override;

private:
    App& app;
    std::vector<Student> students;
    int selectedIndex;
    Button backButton;
    float scrollY;
};

#endif // STUDENT_LIST_SCREEN_HPP

