#include "Database.hpp"
#include <iostream>
#include <sstream>

Database::Database() : db(nullptr) {
}

Database::~Database() {
    close();
}

bool Database::open(const std::string& dbPath) {
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        lastError = sqlite3_errmsg(db);
        sqlite3_close(db);
        db = nullptr;
        return false;
    }
    return true;
}

void Database::close() {
    if (db != nullptr) {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool Database::executeNonQuery(const std::string& sql) {
    if (!db) return false;

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        lastError = errMsg ? errMsg : "Unknown error";
        if (errMsg) sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool Database::executeQuery(const std::string& sql,
                            std::function<int(void*, int, char**, char**)> callback,
                            void* userData) {
    if (!db) return false;

    // Store callback in a way that can be accessed from C callback
    struct CallbackData {
        std::function<int(void*, int, char**, char**)>* func;
        void* userData;
    };
    
    CallbackData cbData = {&callback, userData};

    auto cCallback = [](void* data, int argc, char** argv, char** colNames) -> int {
        CallbackData* cbData = static_cast<CallbackData*>(data);
        return (*(cbData->func))(cbData->userData, argc, argv, colNames);
    };

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), cCallback, &cbData, &errMsg);

    if (rc != SQLITE_OK) {
        lastError = errMsg ? errMsg : "Unknown error";
        if (errMsg) sqlite3_free(errMsg);
        return false;
    }

    return true;
}

std::string Database::getLastError() const {
    return lastError;
}

bool Database::initialize() {
    // Create tables
    std::string schema = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL,
            role TEXT NOT NULL,
            linked_student_id INTEGER
        );

        CREATE TABLE IF NOT EXISTS students (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            student_code TEXT UNIQUE NOT NULL,
            first_name TEXT NOT NULL,
            middle_name TEXT,
            last_name TEXT NOT NULL,
            national_id TEXT,
            passport_no TEXT,
            birth_date TEXT,
            nationality TEXT DEFAULT 'Egyptian',
            gender TEXT,
            address TEXT,
            phone TEXT,
            email TEXT,
            college TEXT,
            program TEXT NOT NULL,
            level INTEGER NOT NULL,
            entry_type TEXT DEFAULT 'fresh',
            previous_university TEXT,
            enrollment_date TEXT,
            academic_status TEXT DEFAULT 'active',
            credits_completed INTEGER DEFAULT 0,
            cgpa REAL DEFAULT 0.0
        );

        CREATE TABLE IF NOT EXISTS courses (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            course_code TEXT UNIQUE NOT NULL,
            title TEXT NOT NULL,
            credits INTEGER NOT NULL
        );

        CREATE TABLE IF NOT EXISTS enrollments (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            student_id INTEGER NOT NULL,
            course_id INTEGER NOT NULL,
            semester TEXT NOT NULL,
            year INTEGER NOT NULL,
            grade REAL,
            FOREIGN KEY(student_id) REFERENCES students(id),
            FOREIGN KEY(course_id) REFERENCES courses(id)
        );

        CREATE TABLE IF NOT EXISTS attendance (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            enrollment_id INTEGER NOT NULL,
            date TEXT NOT NULL,
            status TEXT NOT NULL,
            remark TEXT,
            recorded_by INTEGER,
            FOREIGN KEY(enrollment_id) REFERENCES enrollments(id) ON DELETE CASCADE
        );

        CREATE TABLE IF NOT EXISTS financial_records (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            student_id INTEGER NOT NULL,
            record_type TEXT NOT NULL,
            amount REAL NOT NULL,
            semester TEXT,
            year INTEGER,
            payment_date TEXT,
            status TEXT DEFAULT 'pending',
            receipt_url TEXT,
            notes TEXT,
            FOREIGN KEY(student_id) REFERENCES students(id)
        );

        CREATE TABLE IF NOT EXISTS training_records (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            student_id INTEGER NOT NULL,
            company_name TEXT NOT NULL,
            start_date TEXT NOT NULL,
            end_date TEXT,
            supervisor_name TEXT,
            supervisor_email TEXT,
            report_url TEXT,
            grade REAL,
            evaluation TEXT,
            status TEXT DEFAULT 'ongoing',
            FOREIGN KEY(student_id) REFERENCES students(id)
        );
    )";

    return executeNonQuery(schema);
}

bool Database::seedData() {
    // Check if data already exists
    int count = 0;
    std::string checkSql = "SELECT COUNT(*) FROM students;";
    
    executeQuery(checkSql, [&count](void*, int argc, char** argv, char**) -> int {
        if (argc > 0) {
            count = std::stoi(argv[0]);
        }
        return 0;
    });

    if (count > 0) {
        return true; // Data already seeded
    }

    // Insert seed data
    std::string seedSql = R"(
        -- Users
        INSERT INTO users (username, password_hash, role, linked_student_id) VALUES
        ('admin', 'admin', 'admin', NULL),
        ('NCTU-2024-ICT-001', 'pass123', 'student', 1),
        ('NCTU-2024-ICT-002', 'pass123', 'student', 2);

        -- Students (with complete data)
        INSERT INTO students (student_code, first_name, middle_name, last_name, national_id, birth_date, nationality, gender, address, phone, email, college, program, level, entry_type, enrollment_date, academic_status, credits_completed, cgpa) VALUES
        ('NCTU-2024-ICT-001', 'Ahmed', 'Mohamed', 'Hassan', '29811234567890', '2003-05-15', 'Egyptian', 'male', 'Cairo, Nasr City, 15 Ahmed Fakhry St.', '+201234567890', 'ahmed.hassan@students.nctu.edu.eg', 'كلية تكنولوجيا الصناعة والطاقة', 'ICT', 2, 'fresh', '2024-09-01', 'active', 48, 3.2),
        ('NCTU-2024-ICT-002', 'Sara', 'Ahmed', 'Mohamed', '29911234567891', '2003-08-20', 'Egyptian', 'female', 'Giza, Dokki, 22 Tahrir Square', '+201234567891', 'sara.mohamed@students.nctu.edu.eg', 'كلية تكنولوجيا الصناعة والطاقة', 'ICT', 2, 'fresh', '2024-09-01', 'active', 48, 3.5),
        ('NCTU-2024-MECT-001', 'Omar', 'Ali', 'ElSayed', '29711234567892', '2002-12-10', 'Egyptian', 'male', 'Alexandria, Sidi Bishr, 8 Corniche St.', '+201234567892', 'omar.ali@students.nctu.edu.eg', 'كلية تكنولوجيا الصناعة والطاقة', 'Mechatronics', 3, 'transfer', '2023-09-01', 'active', 72, 3.7);

        -- Courses
        INSERT INTO courses (course_code, title, credits) VALUES
        ('CS101', 'Introduction to Programming', 3),
        ('CS201', 'Data Structures', 3),
        ('MATH101', 'Calculus I', 3),
        ('MECT201', 'Robotics Fundamentals', 3),
        ('ELEC101', 'Basic Electronics', 3);

        -- Enrollments
        INSERT INTO enrollments (student_id, course_id, semester, year, grade) VALUES
        (1, 1, 'Fall', 2024, 85.5),
        (1, 2, 'Fall', 2024, 92.0),
        (1, 3, 'Fall', 2024, 78.5),
        (2, 1, 'Fall', 2024, 90.0),
        (2, 2, 'Fall', 2024, 88.5),
        (3, 4, 'Fall', 2024, 87.0),
        (3, 5, 'Fall', 2024, 91.5);

        -- Attendance (sample records)
        INSERT INTO attendance (enrollment_id, date, status, remark) VALUES
        (1, '2024-09-15', 'present', ''),
        (1, '2024-09-22', 'present', ''),
        (1, '2024-09-29', 'absent', ''),
        (2, '2024-09-15', 'present', ''),
        (2, '2024-09-22', 'present', '');

        -- Financial Records
        INSERT INTO financial_records (student_id, record_type, amount, semester, year, payment_date, status, notes) VALUES
        (1, 'tuition', 5000.0, 'Fall', 2024, '2024-09-05', 'paid', 'Tuition fee for Fall 2024'),
        (1, 'tuition', 5000.0, 'Spring', 2025, NULL, 'pending', 'Tuition fee for Spring 2025'),
        (2, 'tuition', 5000.0, 'Fall', 2024, '2024-09-10', 'paid', 'Tuition fee for Fall 2024'),
        (2, 'scholarship', -2000.0, 'Fall', 2024, '2024-09-01', 'paid', 'Merit scholarship'),
        (3, 'tuition', 5000.0, 'Fall', 2024, '2024-09-08', 'paid', 'Tuition fee for Fall 2024');

        -- Training Records
        INSERT INTO training_records (student_id, company_name, start_date, end_date, supervisor_name, supervisor_email, status, grade, evaluation) VALUES
        (3, 'Egyptian Engineering Company', '2024-06-01', '2024-08-31', 'Eng. Mohamed Ibrahim', 'm.ibrahim@eec.com', 'completed', 95.0, 'Excellent performance and dedication'),
        (1, 'Tech Solutions Egypt', '2025-06-01', NULL, 'Dr. Ahmed Fawzy', 'a.fawzy@techsol.com', 'ongoing', NULL, 'Summer internship');
    )";

    return executeNonQuery(seedSql);
}

