#ifndef RAFEH_SERVICE_HPP
#define RAFEH_SERVICE_HPP

#include "../repositories/RafehRepository.hpp"
#include "../repositories/AuditLogRepository.hpp"
#include "../models/RafehAdjustment.hpp"
#include <vector>

class RafehService {
public:
    RafehService(RafehRepository& repo, AuditLogRepository* auditRepo = nullptr)
        : rafehRepository(repo), auditLogRepository(auditRepo) {}

    bool applyRafeh(int studentId, int year, float amount,
                    const std::string& reason, int appliedBy);
    std::vector<RafehAdjustment> getAdjustmentsForStudent(int studentId);

private:
    RafehRepository& rafehRepository;
    AuditLogRepository* auditLogRepository;
};

#endif // RAFEH_SERVICE_HPP