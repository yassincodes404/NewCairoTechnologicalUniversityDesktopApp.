#ifndef GRADE_COMPONENT_REPOSITORY_HPP
#define GRADE_COMPONENT_REPOSITORY_HPP

#include "../Database.hpp"
#include "../models/GradeComponent.hpp"
#include <vector>

class GradeComponentRepository {
public:
    explicit GradeComponentRepository(Database& db) : database(db) {}

    std::vector<GradeComponent> getByEnrollment(int enrollmentId);
    bool replaceForEnrollment(int enrollmentId, const std::vector<GradeComponent>& components);
    bool deleteForEnrollment(int enrollmentId);

private:
    Database& database;
    GradeComponent rowToGradeComponent(int argc, char** argv, char** colNames);
};

#endif // GRADE_COMPONENT_REPOSITORY_HPP