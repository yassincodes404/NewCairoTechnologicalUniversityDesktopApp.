#ifndef RAFEH_REPOSITORY_HPP
#define RAFEH_REPOSITORY_HPP

#include "../Database.hpp"
#include "../models/RafehAdjustment.hpp"
#include <vector>

class RafehRepository {
public:
    explicit RafehRepository(Database& db) : database(db) {}

    bool insertAdjustment(int studentId, int year, float amount, const std::string& reason, int appliedBy);
    std::vector<RafehAdjustment> getByStudent(int studentId);

private:
    Database& database;
    RafehAdjustment rowToAdjustment(int argc, char** argv, char** colNames);
};

#endif // RAFEH_REPOSITORY_HPP