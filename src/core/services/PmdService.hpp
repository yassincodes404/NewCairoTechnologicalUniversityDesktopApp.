#ifndef PMD_SERVICE_HPP
#define PMD_SERVICE_HPP

#include "../repositories/PmdRepository.hpp"
#include "../repositories/AuditLogRepository.hpp"
#include "../models/PMDRecord.hpp"
#include <vector>

class PmdService {
public:
    PmdService(PmdRepository& repo, AuditLogRepository* auditRepo = nullptr)
        : pmdRepository(repo), auditLogRepository(auditRepo) {}

    int submitRequest(int enrollmentId, const std::string& type,
                      const std::string& docUrl, int submittedBy);
    bool approveRequest(int pmdId, int approverId);
    std::vector<PMDRecord> listPending();

private:
    PmdRepository& pmdRepository;
    AuditLogRepository* auditLogRepository;
};

#endif // PMD_SERVICE_HPP