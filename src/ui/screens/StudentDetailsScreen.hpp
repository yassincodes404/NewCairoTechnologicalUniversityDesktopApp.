#ifndef STUDENT_DETAILS_SCREEN_HPP
#define STUDENT_DETAILS_SCREEN_HPP

#include "../Screen.hpp"
#include "../widgets/Button.hpp"
#include "../../app/App.hpp"
#include "../../core/models/Student.hpp"
#include "../../core/models/Course.hpp"
#include "../../core/models/Enrollment.hpp"
#include "../../core/repositories/PmdRepository.hpp"
#include <vector>
#include <utility>
#include <set>

class StudentDetailsScreen : public Screen {
public:
    explicit StudentDetailsScreen(App& app);
    void handleEvent() override;
    void update(float dt) override;
    void draw() override;

private:
    App& app;
    Student student;
    std::vector<std::pair<Course, Enrollment>> enrollments;
    std::set<int> pmdEnrollmentIds;
    Button backButton;
};

#endif // STUDENT_DETAILS_SCREEN_HPP

