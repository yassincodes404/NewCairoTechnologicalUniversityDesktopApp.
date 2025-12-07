#include "StudentRepository.hpp"
#include <sstream>

std::vector<Student> StudentRepository::getAll() {
    std::vector<Student> students;
    std::string sql = "SELECT id, student_code, first_name, middle_name, last_name, national_id, passport_no, birth_date, nationality, gender, address, phone, email, college, program, level, entry_type, previous_university, enrollment_date, academic_status, credits_completed, cgpa FROM students;";

    database.executeQuery(sql, [&students, this](void*, int argc, char** argv, char** colNames) -> int {
        students.push_back(rowToStudent(argc, argv, colNames));
        return 0;
    });

    return students;
}

std::optional<Student> StudentRepository::getById(int id) {
    std::optional<Student> result;
    std::stringstream sql;
    sql << "SELECT id, student_code, first_name, middle_name, last_name, national_id, passport_no, birth_date, nationality, gender, address, phone, email, college, program, level, entry_type, previous_university, enrollment_date, academic_status, credits_completed, cgpa FROM students WHERE id = " << id << ";";

    database.executeQuery(sql.str(), [&result, this](void*, int argc, char** argv, char** colNames) -> int {
        result = rowToStudent(argc, argv, colNames);
        return 0;
    });

    return result;
}

std::optional<Student> StudentRepository::getByStudentCode(const std::string& code) {
    std::optional<Student> result;
    std::stringstream sql;
    sql << "SELECT id, student_code, first_name, middle_name, last_name, national_id, passport_no, birth_date, nationality, gender, address, phone, email, college, program, level, entry_type, previous_university, enrollment_date, academic_status, credits_completed, cgpa FROM students WHERE student_code = '" << code << "';";

    database.executeQuery(sql.str(), [&result, this](void*, int argc, char** argv, char** colNames) -> int {
        result = rowToStudent(argc, argv, colNames);
        return 0;
    });

    return result;
}

bool StudentRepository::insert(const Student& student) {
    std::stringstream sql;
    sql << "INSERT INTO students (student_code, first_name, middle_name, last_name, national_id, passport_no, birth_date, nationality, gender, address, phone, email, college, program, level, entry_type, previous_university, enrollment_date, academic_status, credits_completed, cgpa) VALUES ("
        << "'" << student.studentCode << "', "
        << "'" << student.firstName << "', "
        << "'" << (student.middleName.empty() ? "" : student.middleName) << "', "
        << "'" << student.lastName << "', "
        << "'" << (student.nationalId.empty() ? "" : student.nationalId) << "', "
        << "'" << (student.passportNo.empty() ? "" : student.passportNo) << "', "
        << "'" << (student.birthDate.empty() ? "" : student.birthDate) << "', "
        << "'" << (student.nationality.empty() ? "Egyptian" : student.nationality) << "', "
        << "'" << (student.gender.empty() ? "" : student.gender) << "', "
        << "'" << (student.address.empty() ? "" : student.address) << "', "
        << "'" << (student.phone.empty() ? "" : student.phone) << "', "
        << "'" << (student.email.empty() ? "" : student.email) << "', "
        << "'" << (student.college.empty() ? "" : student.college) << "', "
        << "'" << student.program << "', "
        << student.level << ", "
        << "'" << (student.entryType.empty() ? "fresh" : student.entryType) << "', "
        << "'" << (student.previousUniversity.empty() ? "" : student.previousUniversity) << "', "
        << "'" << (student.enrollmentDate.empty() ? "" : student.enrollmentDate) << "', "
        << "'" << (student.academicStatus.empty() ? "active" : student.academicStatus) << "', "
        << student.creditsCompleted << ", "
        << student.cgpa << ");";

    return database.executeNonQuery(sql.str());
}

bool StudentRepository::update(const Student& student) {
    std::stringstream sql;
    sql << "UPDATE students SET "
        << "student_code = '" << student.studentCode << "', "
        << "first_name = '" << student.firstName << "', "
        << "middle_name = '" << student.middleName << "', "
        << "last_name = '" << student.lastName << "', "
        << "national_id = '" << student.nationalId << "', "
        << "passport_no = '" << student.passportNo << "', "
        << "birth_date = '" << student.birthDate << "', "
        << "nationality = '" << student.nationality << "', "
        << "gender = '" << student.gender << "', "
        << "address = '" << student.address << "', "
        << "phone = '" << student.phone << "', "
        << "email = '" << student.email << "', "
        << "college = '" << student.college << "', "
        << "program = '" << student.program << "', "
        << "level = " << student.level << ", "
        << "entry_type = '" << student.entryType << "', "
        << "previous_university = '" << student.previousUniversity << "', "
        << "enrollment_date = '" << student.enrollmentDate << "', "
        << "academic_status = '" << student.academicStatus << "', "
        << "credits_completed = " << student.creditsCompleted << ", "
        << "cgpa = " << student.cgpa << " "
        << "WHERE id = " << student.id << ";";

    return database.executeNonQuery(sql.str());
}

bool StudentRepository::remove(int id) {
    std::stringstream sql;
    sql << "DELETE FROM students WHERE id = " << id << ";";
    return database.executeNonQuery(sql.str());
}

Student StudentRepository::rowToStudent(int argc, char** argv, char** colNames) {
    Student s;
    for (int i = 0; i < argc; i++) {
        std::string colName = colNames[i];
        std::string value = argv[i] ? argv[i] : "";

        if (colName == "id") s.id = std::stoi(value);
        else if (colName == "student_code") s.studentCode = value;
        else if (colName == "first_name") s.firstName = value;
        else if (colName == "middle_name") s.middleName = value;
        else if (colName == "last_name") s.lastName = value;
        else if (colName == "national_id") s.nationalId = value;
        else if (colName == "passport_no") s.passportNo = value;
        else if (colName == "birth_date") s.birthDate = value;
        else if (colName == "nationality") s.nationality = value;
        else if (colName == "gender") s.gender = value;
        else if (colName == "address") s.address = value;
        else if (colName == "phone") s.phone = value;
        else if (colName == "email") s.email = value;
        else if (colName == "college") s.college = value;
        else if (colName == "program") s.program = value;
        else if (colName == "level") s.level = std::stoi(value);
        else if (colName == "entry_type") s.entryType = value;
        else if (colName == "previous_university") s.previousUniversity = value;
        else if (colName == "enrollment_date") s.enrollmentDate = value;
        else if (colName == "academic_status") s.academicStatus = value;
        else if (colName == "credits_completed") s.creditsCompleted = std::stoi(value);
        else if (colName == "cgpa") s.cgpa = value.empty() ? 0.0f : std::stof(value);
    }
    return s;
}

