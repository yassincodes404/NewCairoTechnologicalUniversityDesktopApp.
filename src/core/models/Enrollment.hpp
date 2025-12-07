#ifndef ENROLLMENT_HPP
#define ENROLLMENT_HPP

#include <string>
#include <optional>

struct Enrollment {
    int id;
    int studentId;
    int courseId;
    std::string semester;
    int year;
    std::optional<float> grade; // null if not graded yet
};

#endif // ENROLLMENT_HPP

