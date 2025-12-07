#include "CourseService.hpp"

std::vector<Course> CourseService::listCourses() {
    return courseRepository.getAll();
}

std::optional<Course> CourseService::findByCode(const std::string& code) {
    return courseRepository.getByCourseCode(code);
}

std::optional<Course> CourseService::findById(int id) {
    return courseRepository.getById(id);
}

