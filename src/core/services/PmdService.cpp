#include "PmdService.hpp"
#include <sstream>

int PmdService::submitRequest(int enrollmentId, const std::string& type,
                              const std::string& docUrl, int submittedBy) {
    int id = pmdRepository.insertRequest(enrollmentId, type, docUrl);
    if (id <= 0) {
        return -1;
    }

    if (auditLogRepository) {
        std::stringstream diff;
        diff << "{ \"enrollmentId\": " << enrollmentId
             << ", \"type\": \"" << type << "\" }";
        auditLogRepository->insertLog(submittedBy,
                                      "PMD_SUBMIT",
                                      "pmd_records",
                                      id,
                                      diff.str());
    }

    return id;
}

bool PmdService::approveRequest(int pmdId, int approverId) {
    if (!pmdRepository.approve(pmdId, approverId)) {
        return false;
    }

    if (auditLogRepository) {
        std::stringstream diff;
        diff << "{ \"pmdId\": " << pmdId << ", \"status\": \"approved\" }";
        auditLogRepository->insertLog(approverId,
                                      "PMD_APPROVE",
                                      "pmd_records",
                                      pmdId,
                                      diff.str());
    }

    return true;
}

std::vector<PMDRecord> PmdService::listPending() {
    return pmdRepository.getPending();
}