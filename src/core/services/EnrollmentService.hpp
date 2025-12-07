#ifndef ENROLLMENT_SERVICE_HPP
#define ENROLLMENT_SERVICE_HPP

#include "../models/Enrollment.hpp"
#include "../models/GradeComponent.hpp"
#include "../repositories/EnrollmentRepository.hpp"
#include "../repositories/GradeComponentRepository.hpp"
#include "../repositories/AuditLogRepository.hpp"
#include <vector>

class EnrollmentService {
public:
    EnrollmentService(EnrollmentRepository& enrollmentRepo,
                      GradeComponentRepository& gradeRepo,
                      AuditLogRepository* auditRepo = nullptr)
        : enrollmentRepository(enrollmentRepo),
          gradeComponentRepository(gradeRepo),
          auditLogRepository(auditRepo) {}

    std::vector<GradeComponent> getGradeComponents(int enrollmentId);
    bool saveGradeComponents(int enrollmentId,
                             const std::vector<GradeComponent>& components,
                             int actingUserId);

    static float computeFinalFromComponents(const std::vector<GradeComponent>& components);

private:
    EnrollmentRepository& enrollmentRepository;
    GradeComponentRepository& gradeComponentRepository;
    AuditLogRepository* auditLogRepository;
};

#endif // ENROLLMENT_SERVICE_HPP