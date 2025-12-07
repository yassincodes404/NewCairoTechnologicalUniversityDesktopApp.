#include "EnrollmentRepository.hpp"
#include <sstream>

std::vector<Enrollment> EnrollmentRepository::getAll() {
    std::vector<Enrollment> enrollments;
    std::string sql = "SELECT id, student_id, course_id, semester, year, assignment1, assignment2, grade FROM enrollments;";

    database.executeQuery(sql, [&enrollments, this](void*, int argc, char** argv, char** colNames) -> int {
        enrollments.push_back(rowToEnrollment(argc, argv, colNames));
        return 0;
    });

    return enrollments;
}

std::vector<Enrollment> EnrollmentRepository::getByStudentId(int studentId) {
    std::vector<Enrollment> enrollments;
    std::stringstream sql;
    sql << "SELECT id, student_id, course_id, semester, year, assignment1, assignment2, grade FROM enrollments WHERE student_id = " << studentId << ";";

    database.executeQuery(sql.str(), [&enrollments, this](void*, int argc, char** argv, char** colNames) -> int {
        enrollments.push_back(rowToEnrollment(argc, argv, colNames));
        return 0;
    });

    return enrollments;
}

std::vector<std::pair<Course, Enrollment>> EnrollmentRepository::getByStudentIdWithCourses(int studentId) {
    std::vector<std::pair<Course, Enrollment>> results;
    std::stringstream sql;
    sql << "SELECT e.id, e.student_id, e.course_id, e.semester, e.year, e.assignment1, e.assignment2, e.grade, "
        << "c.id as course_id_full, c.course_code, c.title, c.credits "
        << "FROM enrollments e "
        << "JOIN courses c ON e.course_id = c.id "
        << "WHERE e.student_id = " << studentId << ";";

    database.executeQuery(sql.str(), [&results, this](void*, int argc, char** argv, char** colNames) -> int {
        Enrollment e = rowToEnrollment(argc, argv, colNames);
        Course c = rowToCourse(argc, argv, colNames);
        results.push_back({c, e});
        return 0;
    });

    return results;
}

std::optional<Enrollment> EnrollmentRepository::getById(int id) {
    std::optional<Enrollment> result;
    std::stringstream sql;
    sql << "SELECT id, student_id, course_id, semester, year, assignment1, assignment2, grade FROM enrollments WHERE id = " << id << ";";

    database.executeQuery(sql.str(), [&result, this](void*, int argc, char** argv, char** colNames) -> int {
        result = rowToEnrollment(argc, argv, colNames);
        return 0;
    });

    return result;
}

bool EnrollmentRepository::insert(const Enrollment& enrollment) {
    std::stringstream sql;
    sql << "INSERT INTO enrollments (student_id, course_id, semester, year, assignment1, assignment2, grade) VALUES ("
        << enrollment.studentId << ", "
        << enrollment.courseId << ", "
        << "'" << enrollment.semester << "', "
        << enrollment.year << ", ";

    if (enrollment.assignment1.has_value()) {
        sql << enrollment.assignment1.value();
    } else {
        sql << "NULL";
    }
    sql << ", ";

    if (enrollment.assignment2.has_value()) {
        sql << enrollment.assignment2.value();
    } else {
        sql << "NULL";
    }
    sql << ", ";

    if (enrollment.grade.has_value()) {
        sql << enrollment.grade.value();
    } else {
        sql << "NULL";
    }
    sql << ");";

    return database.executeNonQuery(sql.str());
}

bool EnrollmentRepository::update(const Enrollment& enrollment) {
    std::stringstream sql;
    sql << "UPDATE enrollments SET "
        << "student_id = " << enrollment.studentId << ", "
        << "course_id = " << enrollment.courseId << ", "
        << "semester = '" << enrollment.semester << "', "
        << "year = " << enrollment.year << ", ";

    if (enrollment.assignment1.has_value()) {
        sql << "assignment1 = " << enrollment.assignment1.value() << ", ";
    } else {
        sql << "assignment1 = NULL, ";
    }

    if (enrollment.assignment2.has_value()) {
        sql << "assignment2 = " << enrollment.assignment2.value() << ", ";
    } else {
        sql << "assignment2 = NULL, ";
    }

    if (enrollment.grade.has_value()) {
        sql << "grade = " << enrollment.grade.value();
    } else {
        sql << "grade = NULL";
    }
    sql << " WHERE id = " << enrollment.id << ";";

    return database.executeNonQuery(sql.str());
}

bool EnrollmentRepository::remove(int id) {
    std::stringstream sql;
    sql << "DELETE FROM enrollments WHERE id = " << id << ";";
    return database.executeNonQuery(sql.str());
}

Enrollment EnrollmentRepository::rowToEnrollment(int argc, char** argv, char** colNames) {
    Enrollment e;
    for (int i = 0; i < argc; i++) {
        std::string colName = colNames[i];
        std::string value = argv[i] ? argv[i] : "";

        if (colName == "id") e.id = std::stoi(value);
        else if (colName == "student_id") e.studentId = std::stoi(value);
        else if (colName == "course_id") e.courseId = std::stoi(value);
        else if (colName == "semester") e.semester = value;
        else if (colName == "year") e.year = std::stoi(value);
        else if (colName == "assignment1" && !value.empty()) {
            e.assignment1 = std::stof(value);
        }
        else if (colName == "assignment2" && !value.empty()) {
            e.assignment2 = std::stof(value);
        }
        else if (colName == "grade" && !value.empty()) {
            e.grade = std::stof(value);
        }
    }
    return e;
}

Course EnrollmentRepository::rowToCourse(int argc, char** argv, char** colNames) {
    Course c;
    for (int i = 0; i < argc; i++) {
        std::string colName = colNames[i];
        std::string value = argv[i] ? argv[i] : "";

        if (colName == "course_id_full") c.id = std::stoi(value);
        else if (colName == "course_code") c.courseCode = value;
        else if (colName == "title") c.title = value;
        else if (colName == "credits") c.credits = std::stoi(value);
    }
    return c;
}

