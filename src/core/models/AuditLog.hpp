#ifndef AUDIT_LOG_HPP
#define AUDIT_LOG_HPP

#include <string>

struct AuditLog {
    int id;
    int userId;
    std::string action;
    std::string targetTable;
    int targetId;
    std::string diff;
    std::string createdAt;
};

#endif // AUDIT_LOG_HPP