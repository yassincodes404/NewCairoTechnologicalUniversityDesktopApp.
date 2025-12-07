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
    std::optional<float> assignment1; // Assignment 1 mark (nullable)
    std::optional<float> assignment2; // Assignment 2 mark (nullable)
    std::optional<float> grade;       // Final grade (nullable)
};

#endif // ENROLLMENT_HPP

