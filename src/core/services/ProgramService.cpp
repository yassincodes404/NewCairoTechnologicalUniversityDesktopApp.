#include "ProgramService.hpp"

std::vector<Program> ProgramService::listPrograms() {
    return programRepository.getAllPrograms();
}

std::vector<ProgramCourseEntry> ProgramService::getCoursesForLevel(int programId, int level) {
    return programRepository.getProgramCoursesByLevel(programId, level);
}