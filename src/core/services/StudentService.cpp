#include "StudentService.hpp"

std::vector<Student> StudentService::listStudents() {
    return studentRepository.getAll();
}

std::optional<Student> StudentService::findByCode(const std::string& code) {
    return studentRepository.getByStudentCode(code);
}

std::vector<std::pair<Course, Enrollment>> StudentService::getStudentCoursesWithGrades(int studentId) {
    return enrollmentRepository.getByStudentIdWithCourses(studentId);
}

