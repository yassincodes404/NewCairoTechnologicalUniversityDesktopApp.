#ifndef STUDENT_HPP
#define STUDENT_HPP

#include <string>

struct Student {
    int id;
    std::string studentCode;        // الرقم الجامعي
    std::string firstName;          // الاسم الأول
    std::string middleName;          // الاسم الثاني
    std::string lastName;            // اسم العائلة
    std::string nationalId;          // الرقم القومي
    std::string passportNo;          // رقم الجواز (للطلبة الأجانب)
    std::string birthDate;            // تاريخ الميلاد (YYYY-MM-DD)
    std::string nationality;         // الجنسية
    std::string gender;               // النوع (male/female)
    std::string address;              // عنوان الإقامة
    std::string phone;                // رقم الهاتف
    std::string email;                // البريد الإلكتروني
    std::string program;               // البرنامج الدراسي
    std::string college;              // الكلية
    int level;                        // السنة الدراسية (1-4)
    std::string entryType;            // حالة الطالب (fresh/transfer/regular)
    std::string previousUniversity;   // الجامعة السابقة (إذا كان محول)
    std::string enrollmentDate;        // تاريخ الالتحاق
    std::string academicStatus;       // الحالة الأكاديمية (active/probation/suspended/graduated)
    int creditsCompleted;             // الساعات المعتمدة المكتملة
    float cgpa;                       // المعدل التراكمي
};

#endif // STUDENT_HPP
