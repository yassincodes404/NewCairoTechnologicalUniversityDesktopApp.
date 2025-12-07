#ifndef RAFEH_ADJUSTMENT_HPP
#define RAFEH_ADJUSTMENT_HPP

#include <string>

struct RafehAdjustment {
    int id;
    int studentId;
    int year;
    float amount;           // consolation percentage or points
    std::string reason;
    int appliedBy;
    std::string appliedAt;
};

#endif // RAFEH_ADJUSTMENT_HPP