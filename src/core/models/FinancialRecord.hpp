#ifndef FINANCIAL_RECORD_HPP
#define FINANCIAL_RECORD_HPP

#include <string>
#include <optional>

struct FinancialRecord {
    int id;
    int studentId;
    std::string recordType;        // نوع السجل (tuition/fee/scholarship/discount)
    float amount;                  // المبلغ
    std::string semester;          // الفصل الدراسي
    int year;                      // السنة
    std::string paymentDate;       // تاريخ الدفع (YYYY-MM-DD)
    std::string status;            // الحالة (paid/pending/overdue)
    std::string receiptUrl;        // رابط الإيصال (إذا كان موجود)
    std::string notes;             // ملاحظات
};

#endif // FINANCIAL_RECORD_HPP

