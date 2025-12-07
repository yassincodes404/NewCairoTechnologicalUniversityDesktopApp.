#ifndef PMD_RECORD_HPP
#define PMD_RECORD_HPP

#include <string>
#include <optional>

struct PMDRecord {
    int id;
    int enrollmentId;
    std::string pmdType;            // MEDICAL / DEFERRED / PASS
    std::string docUrl;             // supporting document
    std::optional<int> approvedBy;  // NULL = pending
    std::string approvedAt;         // timestamp, empty if not approved
};

#endif // PMD_RECORD_HPP