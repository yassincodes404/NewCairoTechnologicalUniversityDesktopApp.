#include "EnrollmentService.hpp"
#include <sstream>

std::vector<GradeComponent> EnrollmentService::getGradeComponents(int enrollmentId) {
    return gradeComponentRepository.getByEnrollment(enrollmentId);
}

float EnrollmentService::computeFinalFromComponents(const std::vector<GradeComponent>& components) {
    if (components.empty()) {
        return 0.0f;
    }

    float totalWeighted = 0.0f;
    float totalWeight = 0.0f;

    for (const auto& c : components) {
        if (!c.score.has_value() || c.maxScore <= 0.0f) {
            continue;
        }
        float ratio = c.score.value() / c.maxScore;
        totalWeighted += ratio * c.weight;
        totalWeight += c.weight;
    }

    if (totalWeight <= 0.0f) {
        return 0.0f;
    }

    // If weights do not sum to 100, normalize.
    float normalized = totalWeighted * (100.0f / totalWeight);
    return normalized;
}

bool EnrollmentService::saveGradeComponents(int enrollmentId,
                                            const std::vector<GradeComponent>& components,
                                            int actingUserId) {
    auto enrollmentOpt = enrollmentRepository.getById(enrollmentId);
    if (!enrollmentOpt.has_value()) {
        return false;
    }

    if (!gradeComponentRepository.replaceForEnrollment(enrollmentId, components)) {
        return false;
    }

    float finalGrade = computeFinalFromComponents(components);

    Enrollment e = enrollmentOpt.value();
    e.grade = finalGrade;
    if (!enrollmentRepository.update(e)) {
        return false;
    }

    if (auditLogRepository) {
        std::stringstream diff;
        diff << "{ \"enrollmentId\": " << enrollmentId
             << ", \"finalGrade\": " << finalGrade
             << " }";
        auditLogRepository->insertLog(actingUserId,
                                      "GRADE_COMPONENTS_UPDATED",
                                      "enrollments",
                                      enrollmentId,
                                      diff.str());
    }

    return true;
}