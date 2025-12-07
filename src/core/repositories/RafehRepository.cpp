#include "RafehRepository.hpp"
#include <sstream>

bool RafehRepository::insertAdjustment(int studentId, int year, float amount, const std::string& reason, int appliedBy) {
    std::stringstream sql;
    sql << "INSERT INTO rafeh_adjustments (student_id, year, amount, reason, applied_by) VALUES ("
        << studentId << ", "
        << year << ", "
        << amount << ", "
        << "'" << reason << "', "
        << appliedBy << ");";
    return database.executeNonQuery(sql.str());
}

std::vector<RafehAdjustment> RafehRepository::getByStudent(int studentId) {
    std::vector<RafehAdjustment> adjustments;
    std::stringstream sql;
    sql << "SELECT id, student_id, year, amount, reason, applied_by, applied_at "
        << "FROM rafeh_adjustments WHERE student_id = " << studentId << ";";

    database.executeQuery(sql.str(), [&adjustments, this](void*, int argc, char** argv, char** colNames) -> int {
        adjustments.push_back(rowToAdjustment(argc, argv, colNames));
        return 0;
    });

    return adjustments;
}

RafehAdjustment RafehRepository::rowToAdjustment(int argc, char** argv, char** colNames) {
    RafehAdjustment r{};
    for (int i = 0; i < argc; ++i) {
        std::string colName = colNames[i];
        std::string value = argv[i] ? argv[i] : "";

        if (colName == "id") {
            r.id = std::stoi(value);
        } else if (colName == "student_id") {
            r.studentId = std::stoi(value);
        } else if (colName == "year") {
            r.year = std::stoi(value);
        } else if (colName == "amount") {
            r.amount = value.empty() ? 0.0f : std::stof(value);
        } else if (colName == "reason") {
            r.reason = value;
        } else if (colName == "applied_by") {
            r.appliedBy = value.empty() ? 0 : std::stoi(value);
        } else if (colName == "applied_at") {
            r.appliedAt = value;
        }
    }
    return r;
}