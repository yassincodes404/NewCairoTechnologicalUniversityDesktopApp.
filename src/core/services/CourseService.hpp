#ifndef COURSE_SERVICE_HPP
#define COURSE_SERVICE_HPP

#include "../models/Course.hpp"
#include "../repositories/CourseRepository.hpp"
#include <vector>
#include <optional>

class CourseService {
public:
    explicit CourseService(CourseRepository& courseRepo)
        : courseRepository(courseRepo) {}

    std::vector<Course> listCourses();
    std::optional<Course> findByCode(const std::string& code);
    std::optional<Course> findById(int id);

private:
    CourseRepository& courseRepository;
};

#endif // COURSE_SERVICE_HPP

