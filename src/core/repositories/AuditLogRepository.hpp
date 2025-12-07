#ifndef AUDIT_LOG_REPOSITORY_HPP
#define AUDIT_LOG_REPOSITORY_HPP

#include "../Database.hpp"
#include "../models/AuditLog.hpp"
#include <vector>

class AuditLogRepository {
public:
    explicit AuditLogRepository(Database& db) : database(db) {}

    bool insertLog(int userId, const std::string& action,
                   const std::string& targetTable, int targetId,
                   const std::string& diff);
    std::vector<AuditLog> getRecent(int limit);

private:
    Database& database;
    AuditLog rowToAuditLog(int argc, char** argv, char** colNames);
};

#endif // AUDIT_LOG_REPOSITORY_HPP