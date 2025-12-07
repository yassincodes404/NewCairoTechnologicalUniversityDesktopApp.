#include "PmdRepository.hpp"
#include <sstream>

int PmdRepository::insertRequest(int enrollmentId, const std::string& type, const std::string& docUrl) {
    std::stringstream sql;
    sql << "INSERT INTO pmd_records (enrollment_id, pmd_type, doc_url) VALUES ("
        << enrollmentId << ", "
        << "'" << type << "', "
        << "'" << docUrl << "');";

    if (!database.executeNonQuery(sql.str())) {
        return -1;
    }

    // Return last inserted row id
    std::optional<PMDRecord> last;
    std::string query = "SELECT id, enrollment_id, pmd_type, doc_url, approved_by, approved_at "
                        "FROM pmd_records ORDER BY id DESC LIMIT 1;";
    database.executeQuery(query, [&last, this](void*, int argc, char** argv, char** colNames) -> int {
        last = rowToRecord(argc, argv, colNames);
        return 0;
    });

    if (last.has_value()) {
        return last->id;
    }
    return -1;
}

std::vector<PMDRecord> PmdRepository::getByEnrollment(int enrollmentId) {
    std::vector<PMDRecord> records;
    std::stringstream sql;
    sql << "SELECT id, enrollment_id, pmd_type, doc_url, approved_by, approved_at "
        << "FROM pmd_records WHERE enrollment_id = " << enrollmentId << ";";

    database.executeQuery(sql.str(), [&records, this](void*, int argc, char** argv, char** colNames) -> int {
        records.push_back(rowToRecord(argc, argv, colNames));
        return 0;
    });

    return records;
}

std::vector<PMDRecord> PmdRepository::getPending() {
    std::vector<PMDRecord> records;
    std::string sql = "SELECT id, enrollment_id, pmd_type, doc_url, approved_by, approved_at "
                      "FROM pmd_records WHERE approved_by IS NULL;";

    database.executeQuery(sql, [&records, this](void*, int argc, char** argv, char** colNames) -> int {
        records.push_back(rowToRecord(argc, argv, colNames));
        return 0;
    });

    return records;
}

std::optional<PMDRecord> PmdRepository::getById(int id) {
    std::optional<PMDRecord> result;
    std::stringstream sql;
    sql << "SELECT id, enrollment_id, pmd_type, doc_url, approved_by, approved_at "
        << "FROM pmd_records WHERE id = " << id << ";";

    database.executeQuery(sql.str(), [&result, this](void*, int argc, char** argv, char** colNames) -> int {
        result = rowToRecord(argc, argv, colNames);
        return 0;
    });

    return result;
}

bool PmdRepository::approve(int id, int approverId) {
    std::stringstream sql;
    sql << "UPDATE pmd_records SET "
        << "approved_by = " << approverId << ", "
        << "approved_at = datetime('now') "
        << "WHERE id = " << id << ";";
    return database.executeNonQuery(sql.str());
}

PMDRecord PmdRepository::rowToRecord(int argc, char** argv, char** colNames) {
    PMDRecord r{};
    for (int i = 0; i < argc; ++i) {
        std::string colName = colNames[i];
        std::string value = argv[i] ? argv[i] : "";

        if (colName == "id") {
            r.id = std::stoi(value);
        } else if (colName == "enrollment_id") {
            r.enrollmentId = std::stoi(value);
        } else if (colName == "pmd_type") {
            r.pmdType = value;
        } else if (colName == "doc_url") {
            r.docUrl = value;
        } else if (colName == "approved_by" && !value.empty()) {
            r.approvedBy = std::stoi(value);
        } else if (colName == "approved_at") {
            r.approvedAt = value;
        }
    }
    return r;
}