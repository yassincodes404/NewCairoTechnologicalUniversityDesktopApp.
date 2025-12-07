#ifndef PMD_REPOSITORY_HPP
#define PMD_REPOSITORY_HPP

#include "../Database.hpp"
#include "../models/PMDRecord.hpp"
#include <vector>
#include <optional>

class PmdRepository {
public:
    explicit PmdRepository(Database& db) : database(db) {}

    int insertRequest(int enrollmentId, const std::string& type, const std::string& docUrl);
    std::vector<PMDRecord> getByEnrollment(int enrollmentId);
    std::vector<PMDRecord> getPending();
    std::optional<PMDRecord> getById(int id);
    bool approve(int id, int approverId);

private:
    Database& database;
    PMDRecord rowToRecord(int argc, char** argv, char** colNames);
};

#endif // PMD_REPOSITORY_HPP