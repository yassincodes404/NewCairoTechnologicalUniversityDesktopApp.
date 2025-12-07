#ifndef PROGRAM_SERVICE_HPP
#define PROGRAM_SERVICE_HPP

#include "../models/Program.hpp"
#include "../models/Course.hpp"
#include "../repositories/ProgramRepository.hpp"
#include <vector>

class ProgramService {
public:
    explicit ProgramService(ProgramRepository& programRepo)
        : programRepository(programRepo) {}

    std::vector<Program> listPrograms();
    std::vector<ProgramCourseEntry> getCoursesForLevel(int programId, int level);

private:
    ProgramRepository& programRepository;
};

#endif // PROGRAM_SERVICE_HPP