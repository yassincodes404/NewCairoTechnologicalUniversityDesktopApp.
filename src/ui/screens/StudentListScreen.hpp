#ifndef STUDENT_LIST_SCREEN_HPP
#define STUDENT_LIST_SCREEN_HPP

#include "../Screen.hpp"
#include "../widgets/Button.hpp"
#include "../widgets/TextInput.hpp"
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
    std::vector<Student> allStudents;
    std::vector<Student> students;
    int selectedIndex;
    int firstVisibleIndex;
    Button backButton;
    TextInput searchInput;
    float scrollY;
};

#endif // STUDENT_LIST_SCREEN_HPP

