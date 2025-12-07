#ifndef STUDENT_SERVICE_HPP
#define STUDENT_SERVICE_HPP

#include "../models/Student.hpp"
#include "../models/Course.hpp"
#include "../models/Enrollment.hpp"
#include "../repositories/StudentRepository.hpp"
#include "../repositories/EnrollmentRepository.hpp"
#include <vector>
#include <optional>
#include <utility>

class StudentService {
public:
    StudentService(StudentRepository& studentRepo, EnrollmentRepository& enrollmentRepo)
        : studentRepository(studentRepo), enrollmentRepository(enrollmentRepo) {}

    std::vector<Student> listStudents();
    std::optional<Student> findByCode(const std::string& code);
    std::vector<std::pair<Course, Enrollment>> getStudentCoursesWithGrades(int studentId);

private:
    StudentRepository& studentRepository;
    EnrollmentRepository& enrollmentRepository;
};

#endif // STUDENT_SERVICE_HPP

