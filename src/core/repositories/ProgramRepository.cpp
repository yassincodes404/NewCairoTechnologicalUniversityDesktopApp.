#include "ProgramRepository.hpp"
#include <sstream>

std::vector<Program> ProgramRepository::getAllPrograms() {
    std::vector<Program> programs;
    std::string sql = "SELECT id, code, name FROM programs ORDER BY name;";

    database.executeQuery(sql, [&programs, this](void*, int argc, char** argv, char** colNames) -> int {
        programs.push_back(rowToProgram(argc, argv, colNames));
        return 0;
    });

    return programs;
}

std::vector<ProgramCourseEntry> ProgramRepository::getProgramCoursesByLevel(int programId, int level) {
    std::vector<ProgramCourseEntry> entries;
    std::stringstream sql;
    sql << "SELECT "
        << "pc.level AS level_number, "
        << "pc.semester_label, "
        << "c.id AS course_id, c.course_code, c.title, c.credits "
        << "FROM program_courses pc "
        << "JOIN courses c ON pc.course_id = c.id "
        << "WHERE pc.program_id = " << programId << " "
        << "AND pc.level = " << level << " "
        << "ORDER BY pc.semester_label, c.course_code;";

    database.executeQuery(sql.str(), [&entries, this](void*, int argc, char** argv, char** colNames) -> int {
        entries.push_back(rowToProgramCourseEntry(argc, argv, colNames));
        return 0;
    });

    return entries;
}

Program ProgramRepository::rowToProgram(int argc, char** argv, char** colNames) {
    Program p{};
    for (int i = 0; i < argc; i++) {
        std::string colName = colNames[i];
        std::string value = argv[i] ? argv[i] : "";

        if (colName == "id") p.id = std::stoi(value);
        else if (colName == "code") p.code = value;
        else if (colName == "name") p.name = value;
    }
    return p;
}

ProgramCourseEntry ProgramRepository::rowToProgramCourseEntry(int argc, char** argv, char** colNames) {
    ProgramCourseEntry e{};
    for (int i = 0; i < argc; i++) {
        std::string colName = colNames[i];
        std::string value = argv[i] ? argv[i] : "";

        if (colName == "level_number") {
            e.level = std::stoi(value);
        } else if (colName == "semester_label") {
            e.semesterLabel = value;
        } else if (colName == "course_id") {
            e.course.id = std::stoi(value);
        } else if (colName == "course_code") {
            e.course.courseCode = value;
        } else if (colName == "title") {
            e.course.title = value;
        } else if (colName == "credits") {
            e.course.credits = std::stoi(value);
        }
    }
    return e;
}