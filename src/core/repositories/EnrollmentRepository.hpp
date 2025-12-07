#ifndef ENROLLMENT_REPOSITORY_HPP
#define ENROLLMENT_REPOSITORY_HPP

#include "../models/Enrollment.hpp"
#include "../models/Course.hpp"
#include "../Database.hpp"
#include <vector>
#include <optional>
#include <utility>

class EnrollmentRepository {
public:
    explicit EnrollmentRepository(Database& db) : database(db) {}

    std::vector<Enrollment> getAll();
    std::vector<Enrollment> getByStudentId(int studentId);
    std::vector<std::pair<Course, Enrollment>> getByStudentIdWithCourses(int studentId);
    std::optional<Enrollment> getById(int id);
    bool insert(const Enrollment& enrollment);
    bool update(const Enrollment& enrollment);
    bool remove(int id);

private:
    Database& database;
    Enrollment rowToEnrollment(int argc, char** argv, char** colNames);
    Course rowToCourse(int argc, char** argv, char** colNames);
};

#endif // ENROLLMENT_REPOSITORY_HPP

