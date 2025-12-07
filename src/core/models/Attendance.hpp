#ifndef ATTENDANCE_HPP
#define ATTENDANCE_HPP

#include <string>

struct Attendance {
    int id;
    int enrollmentId;        // ربط بالمقرر المسجل
    std::string date;         // تاريخ الحضور (YYYY-MM-DD)
    std::string status;       // الحالة (present/absent/excused)
    std::string remark;       // ملاحظات
    int recordedBy;           // المستخدم الذي سجل الحضور
};

#endif // ATTENDANCE_HPP

