#ifndef PROGRAM_REPOSITORY_HPP
#define PROGRAM_REPOSITORY_HPP

#include "../models/Program.hpp"
#include "../models/Course.hpp"
#include "../Database.hpp"
#include <vector>
#include <string>

struct ProgramCourseEntry {
    Course course;
    int level;
    std::string semesterLabel;
};

class ProgramRepository {
public:
    explicit ProgramRepository(Database& db) : database(db) {}

    std::vector<Program> getAllPrograms();
    std::vector<ProgramCourseEntry> getProgramCoursesByLevel(int programId, int level);

private:
    Database& database;

    Program rowToProgram(int argc, char** argv, char** colNames);
    ProgramCourseEntry rowToProgramCourseEntry(int argc, char** argv, char** colNames);
};

#endif // PROGRAM_REPOSITORY_HPP