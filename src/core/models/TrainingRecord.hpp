#ifndef TRAINING_RECORD_HPP
#define TRAINING_RECORD_HPP

#include <string>
#include <optional>

struct TrainingRecord {
    int id;
    int studentId;
    std::string companyName;       // اسم الشركة
    std::string startDate;          // تاريخ البدء (YYYY-MM-DD)
    std::string endDate;            // تاريخ الانتهاء (YYYY-MM-DD)
    std::string supervisorName;     // اسم المشرف
    std::string supervisorEmail;    // إيميل المشرف
    std::string reportUrl;          // رابط التقرير
    std::optional<float> grade;     // التقييم/الدرجة
    std::string evaluation;         // التقييم النصي
    std::string status;             // الحالة (completed/ongoing/cancelled)
};

#endif // TRAINING_RECORD_HPP

