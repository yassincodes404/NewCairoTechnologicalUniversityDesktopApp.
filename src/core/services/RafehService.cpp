#include "RafehService.hpp"
#include <sstream>

bool RafehService::applyRafeh(int studentId, int year, float amount,
                              const std::string& reason, int appliedBy) {
    if (!rafehRepository.insertAdjustment(studentId, year, amount, reason, appliedBy)) {
        return false;
    }

    if (auditLogRepository) {
        std::stringstream diff;
        diff << "{ \"studentId\": " << studentId
             << ", \"year\": " << year
             << ", \"amount\": " << amount
             << " }";
        auditLogRepository->insertLog(appliedBy,
                                      "RAFEH_APPLY",
                                      "rafeh_adjustments",
                                      studentId,
                                      diff.str());
    }

    return true;
}

std::vector<RafehAdjustment> RafehService::getAdjustmentsForStudent(int studentId) {
    return rafehRepository.getByStudent(studentId);
}