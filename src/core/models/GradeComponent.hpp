#ifndef GRADE_COMPONENT_HPP
#define GRADE_COMPONENT_HPP

#include <string>
#include <optional>

struct GradeComponent {
    int id;
    int enrollmentId;
    std::string componentType;   // e.g. assignment, midterm, final
    float weight;                // percentage weight, should sum to 100
    float maxScore;              // maximum raw score for this component
    std::optional<float> score;  // actual student score (nullable)
    std::optional<int> recordedBy;
    std::string recordedAt;
};

#endif // GRADE_COMPONENT_HPP