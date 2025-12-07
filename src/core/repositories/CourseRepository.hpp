#ifndef COURSE_REPOSITORY_HPP
#define COURSE_REPOSITORY_HPP

#include "../models/Course.hpp"
#include "../Database.hpp"
#include <vector>
#include <optional>

class CourseRepository {
public:
    explicit CourseRepository(Database& db) : database(db) {}

    std::vector<Course> getAll();
    std::optional<Course> getById(int id);
    std::optional<Course> getByCourseCode(const std::string& code);
    bool insert(const Course& course);
    bool update(const Course& course);
    bool remove(int id);

private:
    Database& database;
    Course rowToCourse(int argc, char** argv, char** colNames);
};

#endif // COURSE_REPOSITORY_HPP

