#include "AuditLogRepository.hpp"
#include <sstream>

bool AuditLogRepository::insertLog(int userId, const std::string& action,
                                   const std::string& targetTable, int targetId,
                                   const std::string& diff) {
    std::stringstream sql;
    sql << "INSERT INTO audit_logs (user_id, action, target_table, target_id, diff) VALUES ("
        << userId << ", "
        << "'" << action << "', "
        << "'" << targetTable << "', "
        << targetId << ", "
        << "'" << diff << "');";
    return database.executeNonQuery(sql.str());
}

std::vector<AuditLog> AuditLogRepository::getRecent(int limit) {
    std::vector<AuditLog> logs;
    std::stringstream sql;
    sql << "SELECT id, user_id, action, target_table, target_id, diff, created_at "
        << "FROM audit_logs ORDER BY created_at DESC LIMIT " << limit << ";";

    database.executeQuery(sql.str(), [&logs, this](void*, int argc, char** argv, char** colNames) -> int {
        logs.push_back(rowToAuditLog(argc, argv, colNames));
        return 0;
    });

    return logs;
}

AuditLog AuditLogRepository::rowToAuditLog(int argc, char** argv, char** colNames) {
    AuditLog log{};
    for (int i = 0; i < argc; ++i) {
        std::string colName = colNames[i];
        std::string value = argv[i] ? argv[i] : "";

        if (colName == "id") {
            log.id = std::stoi(value);
        } else if (colName == "user_id") {
            log.userId = value.empty() ? 0 : std::stoi(value);
        } else if (colName == "action") {
            log.action = value;
        } else if (colName == "target_table") {
            log.targetTable = value;
        } else if (colName == "target_id") {
            log.targetId = value.empty() ? 0 : std::stoi(value);
        } else if (colName == "diff") {
            log.diff = value;
        } else if (colName == "created_at") {
            log.createdAt = value;
        }
    }
    return log;
}