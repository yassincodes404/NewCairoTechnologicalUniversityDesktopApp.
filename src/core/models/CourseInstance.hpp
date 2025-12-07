#ifndef COURSE_INSTANCE_HPP
#define COURSE_INSTANCE_HPP

#include <string>
#include <optional>

struct CourseInstance {
    int id;
    int courseId;
    std::optional<int> programId;
    std::string semester;
    int year;
    int term;
    std::string section;
    std::optional<int> instructorId;
    int capacity;
    std::string schedule;
    std::string createdAt;
};

#endif // COURSE_INSTANCE_HPP