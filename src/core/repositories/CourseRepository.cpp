#include "CourseRepository.hpp"
#include <sstream>

std::vector<Course> CourseRepository::getAll() {
    std::vector<Course> courses;
    std::string sql = "SELECT id, course_code, title, credits FROM courses;";

    database.executeQuery(sql, [&courses, this](void*, int argc, char** argv, char** colNames) -> int {
        courses.push_back(rowToCourse(argc, argv, colNames));
        return 0;
    });

    return courses;
}

std::optional<Course> CourseRepository::getById(int id) {
    std::optional<Course> result;
    std::stringstream sql;
    sql << "SELECT id, course_code, title, credits FROM courses WHERE id = " << id << ";";

    database.executeQuery(sql.str(), [&result, this](void*, int argc, char** argv, char** colNames) -> int {
        result = rowToCourse(argc, argv, colNames);
        return 0;
    });

    return result;
}

std::optional<Course> CourseRepository::getByCourseCode(const std::string& code) {
    std::optional<Course> result;
    std::stringstream sql;
    sql << "SELECT id, course_code, title, credits FROM courses WHERE course_code = '" << code << "';";

    database.executeQuery(sql.str(), [&result, this](void*, int argc, char** argv, char** colNames) -> int {
        result = rowToCourse(argc, argv, colNames);
        return 0;
    });

    return result;
}

bool CourseRepository::insert(const Course& course) {
    std::stringstream sql;
    sql << "INSERT INTO courses (course_code, title, credits) VALUES ("
        << "'" << course.courseCode << "', "
        << "'" << course.title << "', "
        << course.credits << ");";

    return database.executeNonQuery(sql.str());
}

bool CourseRepository::update(const Course& course) {
    std::stringstream sql;
    sql << "UPDATE courses SET "
        << "course_code = '" << course.courseCode << "', "
        << "title = '" << course.title << "', "
        << "credits = " << course.credits << " "
        << "WHERE id = " << course.id << ";";

    return database.executeNonQuery(sql.str());
}

bool CourseRepository::remove(int id) {
    std::stringstream sql;
    sql << "DELETE FROM courses WHERE id = " << id << ";";
    return database.executeNonQuery(sql.str());
}

Course CourseRepository::rowToCourse(int argc, char** argv, char** colNames) {
    Course c;
    for (int i = 0; i < argc; i++) {
        std::string colName = colNames[i];
        std::string value = argv[i] ? argv[i] : "";

        if (colName == "id") c.id = std::stoi(value);
        else if (colName == "course_code") c.courseCode = value;
        else if (colName == "title") c.title = value;
        else if (colName == "credits") c.credits = std::stoi(value);
    }
    return c;
}

