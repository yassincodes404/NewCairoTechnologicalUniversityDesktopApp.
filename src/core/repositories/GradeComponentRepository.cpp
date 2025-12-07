#include "GradeComponentRepository.hpp"
#include <sstream>

std::vector<GradeComponent> GradeComponentRepository::getByEnrollment(int enrollmentId) {
    std::vector<GradeComponent> components;
    std::stringstream sql;
    sql << "SELECT id, enrollment_id, component_type, weight, max_score, score, recorded_by, recorded_at "
        << "FROM grade_components WHERE enrollment_id = " << enrollmentId << ";";

    database.executeQuery(sql.str(), [&components, this](void*, int argc, char** argv, char** colNames) -> int {
        components.push_back(rowToGradeComponent(argc, argv, colNames));
        return 0;
    });

    return components;
}

bool GradeComponentRepository::deleteForEnrollment(int enrollmentId) {
    std::stringstream sql;
    sql << "DELETE FROM grade_components WHERE enrollment_id = " << enrollmentId << ";";
    return database.executeNonQuery(sql.str());
}

bool GradeComponentRepository::replaceForEnrollment(int enrollmentId, const std::vector<GradeComponent>& components) {
    // Simple strategy: delete existing components then insert new ones in a transaction
    std::stringstream sql;
    sql << "BEGIN; "
        << "DELETE FROM grade_components WHERE enrollment_id = " << enrollmentId << ";";

    for (const auto& c : components) {
        sql << "INSERT INTO grade_components "
            << "(enrollment_id, component_type, weight, max_score, score, recorded_by) VALUES ("
            << enrollmentId << ", "
            << "'" << c.componentType << "', "
            << c.weight << ", "
            << c.maxScore << ", ";
        if (c.score.has_value()) {
            sql << c.score.value();
        } else {
            sql << "NULL";
        }
        sql << ", ";
        if (c.recordedBy.has_value()) {
            sql << c.recordedBy.value();
        } else {
            sql << "NULL";
        }
        sql << ");";
    }

    sql << "COMMIT;";

    return database.executeNonQuery(sql.str());
}

GradeComponent GradeComponentRepository::rowToGradeComponent(int argc, char** argv, char** colNames) {
    GradeComponent c{};
    for (int i = 0; i < argc; ++i) {
        std::string colName = colNames[i];
        std::string value = argv[i] ? argv[i] : "";

        if (colName == "id") {
            c.id = std::stoi(value);
        } else if (colName == "enrollment_id") {
            c.enrollmentId = std::stoi(value);
        } else if (colName == "component_type") {
            c.componentType = value;
        } else if (colName == "weight") {
            c.weight = value.empty() ? 0.0f : std::stof(value);
        } else if (colName == "max_score") {
            c.maxScore = value.empty() ? 0.0f : std::stof(value);
        } else if (colName == "score" && !value.empty()) {
            c.score = std::stof(value);
        } else if (colName == "recorded_by" && !value.empty()) {
            c.recordedBy = std::stoi(value);
        } else if (colName == "recorded_at") {
            c.recordedAt = value;
        }
    }
    return c;
}