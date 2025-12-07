#ifndef STUDENT_REPOSITORY_HPP
#define STUDENT_REPOSITORY_HPP

#include "../models/Student.hpp"
#include "../Database.hpp"
#include <vector>
#include <optional>

class StudentRepository {
public:
    explicit StudentRepository(Database& db) : database(db) {}

    std::vector<Student> getAll();
    std::optional<Student> getById(int id);
    std::optional<Student> getByStudentCode(const std::string& code);
    bool insert(const Student& student);
    bool update(const Student& student);
    bool remove(int id);

private:
    Database& database;
    Student rowToStudent(int argc, char** argv, char** colNames);
};

#endif // STUDENT_REPOSITORY_HPP

