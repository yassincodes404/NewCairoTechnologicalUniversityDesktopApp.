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
            assignment1 REAL,
            assignment2 REAL,
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

        -- Academic programs at NCTU (Mechatronics, Autotronics, Renewable Energy, ICT, Prosthetics & Orthotics, Petroleum)
        CREATE TABLE IF NOT EXISTS programs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            code TEXT UNIQUE NOT NULL,
            name TEXT NOT NULL
        );

        -- Curriculum mapping: which courses belong to which program, level and semester group
        CREATE TABLE IF NOT EXISTS program_courses (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            program_id INTEGER NOT NULL,
            course_id INTEGER NOT NULL,
            level INTEGER NOT NULL,
            semester_label TEXT NOT NULL,
            UNIQUE(program_id, course_id, level, semester_label),
            FOREIGN KEY(program_id) REFERENCES programs(id),
            FOREIGN KEY(course_id) REFERENCES courses(id)
        );

        -- Faculty members (instructors)
        CREATE TABLE IF NOT EXISTS faculty (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            employee_code TEXT UNIQUE,
            title TEXT,
            first_name TEXT,
            last_name TEXT,
            email TEXT,
            phone TEXT,
            department TEXT,
            degrees TEXT
        );

        -- Specific course offerings (per semester, year, section)
        CREATE TABLE IF NOT EXISTS course_instances (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            course_id INTEGER NOT NULL,
            program_id INTEGER,
            semester TEXT,
            year INTEGER,
            term INTEGER,
            section TEXT,
            instructor_id INTEGER,
            capacity INTEGER,
            schedule TEXT,
            created_at TEXT DEFAULT (datetime('now')),
            FOREIGN KEY(course_id) REFERENCES courses(id),
            FOREIGN KEY(program_id) REFERENCES programs(id),
            FOREIGN KEY(instructor_id) REFERENCES faculty(id)
        );

        -- Fine-grained grade components per enrollment (assignment, midterm, final, etc.)
        CREATE TABLE IF NOT EXISTS grade_components (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            enrollment_id INTEGER NOT NULL,
            component_type TEXT,
            weight REAL,
            max_score REAL,
            score REAL,
            recorded_by INTEGER,
            recorded_at TEXT DEFAULT (datetime('now')),
            FOREIGN KEY(enrollment_id) REFERENCES enrollments(id)
        );

        -- PMD (medical/deferred/pass) records linked to enrollments
        CREATE TABLE IF NOT EXISTS pmd_records (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            enrollment_id INTEGER NOT NULL,
            pmd_type TEXT,
            doc_url TEXT,
            approved_by INTEGER,
            approved_at TEXT,
            FOREIGN KEY(enrollment_id) REFERENCES enrollments(id)
        );

        -- Rafeh (consolation mark) adjustments per student per year
        CREATE TABLE IF NOT EXISTS rafeh_adjustments (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            student_id INTEGER,
            year INTEGER,
            amount REAL,
            reason TEXT,
            applied_by INTEGER,
            applied_at TEXT DEFAULT (datetime('now')),
            FOREIGN KEY(student_id) REFERENCES students(id)
        );

        -- Transcript snapshots
        CREATE TABLE IF NOT EXISTS transcripts (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            student_id INTEGER,
            generated_at TEXT DEFAULT (datetime('now')),
            content TEXT,
            pdf_url TEXT,
            FOREIGN KEY(student_id) REFERENCES students(id)
        );

        -- Generic documents (admission docs, medical reports, etc.)
        CREATE TABLE IF NOT EXISTS documents (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            owner_id INTEGER,
            doc_type TEXT,
            file_url TEXT,
            file_hash TEXT,
            uploaded_by INTEGER,
            uploaded_at TEXT DEFAULT (datetime('now'))
        );

        -- Audit log for important actions
        CREATE TABLE IF NOT EXISTS audit_logs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER,
            action TEXT,
            target_table TEXT,
            target_id INTEGER,
            diff TEXT,
            created_at TEXT DEFAULT (datetime('now'))
        );

        -- Simple notifications table (optional)
        CREATE TABLE IF NOT EXISTS notifications (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER,
            title TEXT,
            message TEXT,
            is_read INTEGER DEFAULT 0,
            created_at TEXT DEFAULT (datetime('now'))
        );
    )";

    return executeNonQuery(schema);
}

bool Database::seedData() {
    // 1) Seed academic structure (programs, courses, curriculum)
    //    This is idempotent and will run even if demo students already exist.

    std::string structureSql = R"(
        --------------------------------------------------------------------
        -- Programs (Fields)
        --------------------------------------------------------------------
        INSERT OR IGNORE INTO programs (code, name) VALUES
        ('MECT',     'Mechatronics Technology'),
        ('AUTOTRON', 'Autotronics Technology'),
        ('RENEW',    'Renewable Energy Technology'),
        ('ICT',      'Information & Communication Technology (ICT)'),
        ('PROS',     'Prosthetics & Orthotics Technology'),
        ('PETRO',    'Petroleum Technology');

        --------------------------------------------------------------------
        -- Courses (shared catalog for all programs)
        -- NOTE: course_code remains unique across the university.
        --------------------------------------------------------------------
        INSERT OR IGNORE INTO courses (course_code, title, credits) VALUES
        -- Foundation / General courses
        ('MATH101',       'Calculus I / Math for Computing',                         3),
        ('MATH102',       'Linear Algebra and Discrete Math',                        3),
        ('PHYS101',       'General Physics I / Physics',                             3),
        ('PHYS102',       'General Physics II',                                      3),
        ('CS101',         'Programming I (C/C++)',                                   3),
        ('CS102',         'Programming II (OOP)',                                    3),
        ('ENG101',        'Technical English I',                                     2),
        ('ENG102',        'Technical Communication Skills',                          2),
        ('ENG_IT101',     'English for IT',                                          2),
        ('ICT101',        'Introduction to Communication Systems',                   3),
        ('WORK101',       'Mechanical Workshop',                                     2),
        ('IT101',         'IT and Computer Skills',                                  3),
        ('CHEM101',       'General Chemistry',                                       3),
        ('GEO101',        'Introduction to Geology',                                 3),

        -- Core engineering / mechatronics courses
        ('ELEC101',       'Fundamentals of Electrical Circuits',                     3),
        ('ELEC201',       'Electrical Circuits II',                                  3),
        ('ELEC202',       'Electronics I',                                           3),
        ('ELEC203',       'Power Electronics',                                       3),
        ('MECH201',       'Engineering Mechanics',                                   3),
        ('CAD201',        'Engineering Drawing and AutoCAD',                         3),
        ('MEAS201',       'Measurement and Instrumentation',                         3),
        ('CONTROL201',    'Automatic Control I',                                     3),
        ('FLUIDS201',     'Fluid Mechanics',                                         3),
        ('EMBED201',      'Introduction to Embedded Systems',                        3),
        ('ELEC_LAB',      'Electrical Laboratory',                                   2),

        -- Mechatronics specialization
        ('ROBOT301',      'Robotics I',                                              3),
        ('EMBED301',      'Advanced Embedded Systems',                               3),
        ('ACTU301',       'Actuators and Electric Drives',                           3),
        ('PLC301',        'PLC and Industrial Control',                              3),
        ('PROJECT301',    'Applied Project I',                                       3),
        ('ROBOT302',      'Robotics II',                                             3),
        ('SENS302',       'Sensors and Advanced Instrumentation',                    3),
        ('MAINT302',      'Industrial Maintenance',                                  3),
        ('IOT302',        'Industrial IoT Systems',                                  3),
        ('ROBOT_LAB',     'Robotics Laboratory',                                     2),
        ('ADVCTRL401',    'Advanced Control Systems',                                3),
        ('AI401',         'Intelligent Systems and AI Applications',                 3),
        ('SAF401',        'Industrial Safety',                                       2),
        ('TRAIN401',      'Internship / Industrial Training I',                      2),
        ('GRAD401',       'Graduation Project',                                      4),
        ('ENT401',        'Technological Entrepreneurship',                          2),
        ('TRAIN402',      'Internship / Industrial Evaluation II',                   2),

        -- Autotronics courses
        ('AUTO201',       'Internal Combustion Engines',                             3),
        ('AUTO202',       'Automotive Electrical Systems',                           3),
        ('ELEC_AUTO201',  'Automotive Electronics',                                  3),
        ('CAN201',        'CAN Bus and Automotive Networks',                         3),
        ('MECH_AUTO201',  'Vehicle Dynamics',                                        3),
        ('AUTO301',       'Fuel Injection Systems',                                  3),
        ('AUTO302',       'ABS/ESP Brake Systems',                                   3),
        ('AUTO303',       'Automatic Transmission Systems',                          3),
        ('AUTO304',       'OBD and Diagnostic Systems',                              3),
        ('PROJECT_AUTO1', 'Applied Automotive Project',                              3),
        ('EV401',         'Electric and Hybrid Vehicles',                            3),
        ('ADAS401',       'Introduction to Advanced Driver Assistance Systems',      3),
        ('TRAIN_AUTO',    'Industry Training (Autotronics)',                         2),
        ('GRAD_AUTO',     'Graduation Project (Automotive System)',                  4),

        -- Renewable Energy courses
        ('ELEC_EN201',    'Electrical Systems for Energy',                           3),
        ('THERM201',      'Thermodynamics and Heat Transfer',                        3),
        ('ENERG201',      'Introduction to Energy Systems',                          3),
        ('MEAS_EN201',    'Energy Measurement Techniques',                           3),
        ('SOLAR301',      'Photovoltaic Solar Energy Systems',                       3),
        ('WIND301',       'Wind Energy Systems',                                     3),
        ('POWER_CONV301', 'Power Converters for Renewable Energy',                   3),
        ('GRID301',       'Grid Integration of Renewable Sources',                   3),
        ('SMARTGRID401',  'Smart Grids',                                             3),
        ('ECON401',       'Energy Economics',                                        3),
        ('TRAIN_EN',      'Industrial Training in Energy Sector',                    2),
        ('GRAD_EN',       'Graduation Project (Renewable Energy)',                   4),

        -- ICT program courses
        ('NET101',        'Networks I',                                              3),
        ('OS101',         'Operating Systems (Introduction)',                        3),
        ('DB201',         'Databases I',                                             3),
        ('WEB201',        'Web Development I',                                       3),
        ('NET202',        'Networks II',                                             3),
        ('SEC201',        'Introduction to Cybersecurity',                           3),
        ('DS201',         'Data Structures and Algorithms',                          3),
        ('DB301',         'Advanced Databases',                                      3),
        ('MOBILE301',     'Mobile Application Development',                          3),
        ('CLOUD301',      'Cloud Computing Basics',                                  3),
        ('IOT301',        'IoT and Embedded Networking',                             3),
        ('AI301',         'Applied Artificial Intelligence',                         3),
        ('DEVOPS401',     'DevOps and CI/CD',                                        3),
        ('NETSEC401',     'Advanced Network Security',                               3),
        ('TRAIN_ICT',     'ICT Internship',                                          2),
        ('GRAD_ICT',      'Graduation Project (Web/Mobile/IoT)',                     4),

        -- Prosthetics & Orthotics courses
        ('ANAT101',       'Anatomy and Physiology',                                  3),
        ('MAT_MED101',    'Medical Materials',                                       3),
        ('BIO101',        'Biomechanics Basics',                                     3),
        ('BIO201',        'Applied Biomechanics',                                    3),
        ('CAD_MED201',    '3D CAD for Prosthetics',                                  3),
        ('MANUF201',      'Manufacturing Processes for Prosthetics',                 3),
        ('MEDDEV201',     'Medical Device Basics',                                   3),
        ('PROS301',       'Lower Limb Prosthetics',                                  3),
        ('PROS302',       'Upper Limb Prosthetics',                                  3),
        ('ORTHO301',      'Orthotic Devices',                                        3),
        ('REHAB301',      'Rehabilitation Techniques',                               3),
        ('CLIN401',       'Clinical Field Training',                                 3),
        ('GRAD_PROS',     'Graduation Project (Prosthetics and Orthotics)',          4),

        -- Petroleum Technology courses
        ('GEO201',        'Petroleum Geology',                                       3),
        ('DRILL201',      'Drilling Fundamentals',                                   3),
        ('FLUIDS_OIL201', 'Petroleum Fluids',                                        3),
        ('SAF_PET201',    'Industrial Safety in Oil Sector',                         3),
        ('DRILL301',      'Advanced Drilling',                                       3),
        ('PROD301',       'Oil Production and Processing',                           3),
        ('GAS301',        'Natural Gas Technology',                                  3),
        ('PIPE301',       'Pipeline Transportation',                                 3),
        ('FIELD401',      'Field Training (Oil and Gas)',                            3),
        ('GRAD_PETRO',    'Graduation Project (Petroleum Technology)',               4),

        -- Existing demo-specific courses (kept for compatibility)
        ('CS201',         'Data Structures',                                         3),
        ('MECT201',       'Robotics Fundamentals',                                   3);

        --------------------------------------------------------------------
        -- Curriculum mapping: program, level (1-4), and semester group
        --------------------------------------------------------------------

        -- Mechatronics Technology (MECT) - Level 1, Semester 1
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'MATH101'), 1, 'Semester 1');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'PHYS101'), 1, 'Semester 1');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'CS101'),   1, 'Semester 1');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'ENG101'),  1, 'Semester 1');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'WORK101'), 1, 'Semester 1');

        -- Mechatronics Technology (MECT) - Level 1, Semester 2
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'MATH102'), 1, 'Semester 2');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'PHYS102'), 1, 'Semester 2');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'ELEC101'), 1, 'Semester 2');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'IT101'),   1, 'Semester 2');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'ENG102'),  1, 'Semester 2');

        -- Mechatronics Technology (MECT) - Level 2, Semester 3
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'ELEC201'), 2, 'Semester 3');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'ELEC202'), 2, 'Semester 3');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'MECH201'), 2, 'Semester 3');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'CAD201'),  2, 'Semester 3');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'MEAS201'), 2, 'Semester 3');

        -- Mechatronics Technology (MECT) - Level 2, Semester 4
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'CONTROL201'), 2, 'Semester 4');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'ELEC203'),    2, 'Semester 4');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'FLUIDS201'),  2, 'Semester 4');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'EMBED201'),   2, 'Semester 4');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'ELEC_LAB'),   2, 'Semester 4');

        -- Mechatronics Technology (MECT) - Level 3, Semester 5
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'ROBOT301'),   3, 'Semester 5');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'EMBED301'),   3, 'Semester 5');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'ACTU301'),    3, 'Semester 5');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'PLC301'),     3, 'Semester 5');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'PROJECT301'), 3, 'Semester 5');

        -- Mechatronics Technology (MECT) - Level 3, Semester 6
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'ROBOT302'),   3, 'Semester 6');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'SENS302'),    3, 'Semester 6');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'MAINT302'),   3, 'Semester 6');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'IOT302'),     3, 'Semester 6');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'ROBOT_LAB'),  3, 'Semester 6');

        -- Mechatronics Technology (MECT) - Level 4, Semester 7
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'ADVCTRL401'), 4, 'Semester 7');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'AI401'),      4, 'Semester 7');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'SAF401'),     4, 'Semester 7');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'TRAIN401'),   4, 'Semester 7');

        -- Mechatronics Technology (MECT) - Level 4, Semester 8
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'GRAD401'),    4, 'Semester 8');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'ENT401'),     4, 'Semester 8');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'MECT'), (SELECT id FROM courses WHERE course_code = 'TRAIN402'),   4, 'Semester 8');

        --------------------------------------------------------------------
        -- Autotronics Technology (AUTOTRON)
        --------------------------------------------------------------------

        -- AUTOTRON - Level 1 Foundation (same as Mechatronics)
        -- Semester 1
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'MATH101'), 1, 'Semester 1');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'PHYS101'), 1, 'Semester 1');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'CS101'),   1, 'Semester 1');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'ENG101'),  1, 'Semester 1');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'WORK101'), 1, 'Semester 1');

        -- AUTOTRON - Level 1, Semester 2
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'MATH102'), 1, 'Semester 2');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'PHYS102'), 1, 'Semester 2');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'ELEC101'), 1, 'Semester 2');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'IT101'),   1, 'Semester 2');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'ENG102'),  1, 'Semester 2');

        -- AUTOTRON - Level 2, Sem 3-4 (Core Automotive)
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'AUTO201'),      2, 'Sem 3-4');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'AUTO202'),      2, 'Sem 3-4');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'ELEC_AUTO201'), 2, 'Sem 3-4');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'CAN201'),       2, 'Sem 3-4');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'MECH_AUTO201'), 2, 'Sem 3-4');

        -- AUTOTRON - Level 3, Sem 5-6 (Advanced Automotive)
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'AUTO301'),      3, 'Sem 5-6');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'AUTO302'),      3, 'Sem 5-6');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'AUTO303'),      3, 'Sem 5-6');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'AUTO304'),      3, 'Sem 5-6');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'PROJECT_AUTO1'),3, 'Sem 5-6');

        -- AUTOTRON - Level 4, Sem 7-8 (EV and Smart Vehicles)
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'EV401'),        4, 'Sem 7-8');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'ADAS401'),      4, 'Sem 7-8');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'TRAIN_AUTO'),   4, 'Sem 7-8');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'AUTOTRON'), (SELECT id FROM courses WHERE course_code = 'GRAD_AUTO'),    4, 'Sem 7-8');

        --------------------------------------------------------------------
        -- Renewable Energy Technology (RENEW)
        --------------------------------------------------------------------

        -- RENEW - Level 1 Foundation (similar structure -> use common foundation)
        -- Semester 1
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'RENEW'), (SELECT id FROM courses WHERE course_code = 'MATH101'), 1, 'Semester 1');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'RENEW'), (SELECT id FROM courses WHERE course_code = 'PHYS101'), 1, 'Semester 1');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'RENEW'), (SELECT id FROM courses WHERE course_code = 'CS101'),   1, 'Semester 1');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'RENEW'), (SELECT id FROM courses WHERE course_code = 'ENG101'),  1, 'Semester 1');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'RENEW'), (SELECT id FROM courses WHERE course_code = 'WORK101'), 1, 'Semester 1');

        -- RENEW - Level 1, Semester 2
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'RENEW'), (SELECT id FROM courses WHERE course_code = 'MATH102'), 1, 'Semester 2');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'RENEW'), (SELECT id FROM courses WHERE course_code = 'PHYS102'), 1, 'Semester 2');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'RENEW'), (SELECT id FROM courses WHERE course_code = 'ELEC101'), 1, 'Semester 2');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'RENEW'), (SELECT id FROM courses WHERE course_code = 'IT101'),   1, 'Semester 2');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'RENEW'), (SELECT id FROM courses WHERE course_code = 'ENG102'),  1, 'Semester 2');

        -- RENEW - Level 2, Sem 3-4 (Energy Fundamentals)
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'RENEW'), (SELECT id FROM courses WHERE course_code = 'ELEC_EN201'), 2, 'Sem 3-4');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'RENEW'), (SELECT id FROM courses WHERE course_code = 'THERM201'),    2, 'Sem 3-4');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'RENEW'), (SELECT id FROM courses WHERE course_code = 'ENERG201'),    2, 'Sem 3-4');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'RENEW'), (SELECT id FROM courses WHERE course_code = 'MEAS_EN201'),  2, 'Sem 3-4');

        -- RENEW - Level 3, Sem 5-6 (Renewable Systems)
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'RENEW'), (SELECT id FROM courses WHERE course_code = 'SOLAR301'),     3, 'Sem 5-6');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'RENEW'), (SELECT id FROM courses WHERE course_code = 'WIND301'),      3, 'Sem 5-6');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'RENEW'), (SELECT id FROM courses WHERE course_code = 'POWER_CONV301'),3, 'Sem 5-6');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'RENEW'), (SELECT id FROM courses WHERE course_code = 'GRID301'),      3, 'Sem 5-6');

        -- RENEW - Level 4, Sem 7-8
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'RENEW'), (SELECT id FROM courses WHERE course_code = 'SMARTGRID401'), 4, 'Sem 7-8');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'RENEW'), (SELECT id FROM courses WHERE course_code = 'ECON401'),      4, 'Sem 7-8');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'RENEW'), (SELECT id FROM courses WHERE course_code = 'TRAIN_EN'),     4, 'Sem 7-8');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'RENEW'), (SELECT id FROM courses WHERE course_code = 'GRAD_EN'),      4, 'Sem 7-8');

        --------------------------------------------------------------------
        -- Information & Communication Technology (ICT)
        --------------------------------------------------------------------

        -- ICT - Level 1, Semester 1 (IT Foundation)
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'ICT'), (SELECT id FROM courses WHERE course_code = 'CS101'),      1, 'Semester 1');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'ICT'), (SELECT id FROM courses WHERE course_code = 'MATH101'),    1, 'Semester 1');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'ICT'), (SELECT id FROM courses WHERE course_code = 'ENG_IT101'),  1, 'Semester 1');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'ICT'), (SELECT id FROM courses WHERE course_code = 'ICT101'),     1, 'Semester 1');

        -- ICT - Level 1, Semester 2
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'ICT'), (SELECT id FROM courses WHERE course_code = 'CS102'),      1, 'Semester 2');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'ICT'), (SELECT id FROM courses WHERE course_code = 'MATH102'),    1, 'Semester 2');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'ICT'), (SELECT id FROM courses WHERE course_code = 'NET101'),     1, 'Semester 2');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'ICT'), (SELECT id FROM courses WHERE course_code = 'OS101'),      1, 'Semester 2');

        -- ICT - Level 2, Sem 3-4 (Core ICT)
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'ICT'), (SELECT id FROM courses WHERE course_code = 'DB201'),      2, 'Sem 3-4');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'ICT'), (SELECT id FROM courses WHERE course_code = 'WEB201'),     2, 'Sem 3-4');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'ICT'), (SELECT id FROM courses WHERE course_code = 'NET202'),     2, 'Sem 3-4');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'ICT'), (SELECT id FROM courses WHERE course_code = 'SEC201'),     2, 'Sem 3-4');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'ICT'), (SELECT id FROM courses WHERE course_code = 'DS201'),      2, 'Sem 3-4');

        -- ICT - Level 3, Sem 5-6 (Advanced ICT)
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'ICT'), (SELECT id FROM courses WHERE course_code = 'DB301'),      3, 'Sem 5-6');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'ICT'), (SELECT id FROM courses WHERE course_code = 'MOBILE301'),  3, 'Sem 5-6');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'ICT'), (SELECT id FROM courses WHERE course_code = 'CLOUD301'),   3, 'Sem 5-6');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'ICT'), (SELECT id FROM courses WHERE course_code = 'IOT301'),     3, 'Sem 5-6');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'ICT'), (SELECT id FROM courses WHERE course_code = 'AI301'),      3, 'Sem 5-6');

        -- ICT - Level 4, Sem 7-8
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'ICT'), (SELECT id FROM courses WHERE course_code = 'DEVOPS401'),  4, 'Sem 7-8');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'ICT'), (SELECT id FROM courses WHERE course_code = 'NETSEC401'),  4, 'Sem 7-8');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'ICT'), (SELECT id FROM courses WHERE course_code = 'TRAIN_ICT'),  4, 'Sem 7-8');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'ICT'), (SELECT id FROM courses WHERE course_code = 'GRAD_ICT'),   4, 'Sem 7-8');

        --------------------------------------------------------------------
        -- Prosthetics & Orthotics Technology (PROS)
        --------------------------------------------------------------------

        -- PROS - Level 1, Sem 1-2
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PROS'), (SELECT id FROM courses WHERE course_code = 'ANAT101'),   1, 'Sem 1-2');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PROS'), (SELECT id FROM courses WHERE course_code = 'MAT_MED101'),1, 'Sem 1-2');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PROS'), (SELECT id FROM courses WHERE course_code = 'BIO101'),    1, 'Sem 1-2');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PROS'), (SELECT id FROM courses WHERE course_code = 'IT101'),     1, 'Sem 1-2');

        -- PROS - Level 2, Sem 3-4
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PROS'), (SELECT id FROM courses WHERE course_code = 'BIO201'),     2, 'Sem 3-4');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PROS'), (SELECT id FROM courses WHERE course_code = 'CAD_MED201'), 2, 'Sem 3-4');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PROS'), (SELECT id FROM courses WHERE course_code = 'MANUF201'),   2, 'Sem 3-4');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PROS'), (SELECT id FROM courses WHERE course_code = 'MEDDEV201'),  2, 'Sem 3-4');

        -- PROS - Level 3, Sem 5-6
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PROS'), (SELECT id FROM courses WHERE course_code = 'PROS301'),    3, 'Sem 5-6');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PROS'), (SELECT id FROM courses WHERE course_code = 'PROS302'),    3, 'Sem 5-6');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PROS'), (SELECT id FROM courses WHERE course_code = 'ORTHO301'),   3, 'Sem 5-6');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PROS'), (SELECT id FROM courses WHERE course_code = 'REHAB301'),   3, 'Sem 5-6');

        -- PROS - Level 4, Sem 7-8
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PROS'), (SELECT id FROM courses WHERE course_code = 'CLIN401'),    4, 'Sem 7-8');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PROS'), (SELECT id FROM courses WHERE course_code = 'GRAD_PROS'),  4, 'Sem 7-8');

        --------------------------------------------------------------------
        -- Petroleum Technology (PETRO)
        --------------------------------------------------------------------

        -- PETRO - Level 1, Sem 1-2
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PETRO'), (SELECT id FROM courses WHERE course_code = 'CHEM101'),  1, 'Sem 1-2');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PETRO'), (SELECT id FROM courses WHERE course_code = 'PHYS101'),  1, 'Sem 1-2');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PETRO'), (SELECT id FROM courses WHERE course_code = 'GEO101'),   1, 'Sem 1-2');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PETRO'), (SELECT id FROM courses WHERE course_code = 'IT101'),    1, 'Sem 1-2');

        -- PETRO - Level 2, Sem 3-4
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PETRO'), (SELECT id FROM courses WHERE course_code = 'GEO201'),       2, 'Sem 3-4');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PETRO'), (SELECT id FROM courses WHERE course_code = 'DRILL201'),     2, 'Sem 3-4');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PETRO'), (SELECT id FROM courses WHERE course_code = 'FLUIDS_OIL201'),2, 'Sem 3-4');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PETRO'), (SELECT id FROM courses WHERE course_code = 'SAF_PET201'),   2, 'Sem 3-4');

        -- PETRO - Level 3, Sem 5-6
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PETRO'), (SELECT id FROM courses WHERE course_code = 'DRILL301'), 3, 'Sem 5-6');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PETRO'), (SELECT id FROM courses WHERE course_code = 'PROD301'),  3, 'Sem 5-6');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PETRO'), (SELECT id FROM courses WHERE course_code = 'GAS301'),   3, 'Sem 5-6');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PETRO'), (SELECT id FROM courses WHERE course_code = 'PIPE301'),  3, 'Sem 5-6');

        -- PETRO - Level 4, Sem 7-8
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PETRO'), (SELECT id FROM courses WHERE course_code = 'FIELD401'),   4, 'Sem 7-8');
        INSERT OR IGNORE INTO program_courses (program_id, course_id, level, semester_label) VALUES
        ((SELECT id FROM programs WHERE code = 'PETRO'), (SELECT id FROM courses WHERE course_code = 'GRAD_PETRO'), 4, 'Sem 7-8');
    )";

    if (!executeNonQuery(structureSql)) {
        return false;
    }

    // 2) Seed demo users/students/enrollments/etc only once (if no students yet)
    int count = 0;
    std::string checkSql = "SELECT COUNT(*) FROM students;";
    
    executeQuery(checkSql, [&count](void*, int argc, char** argv, char**) -> int {
        if (argc > 0) {
            count = std::stoi(argv[0]);
        }
        return 0;
    });

    if (count > 0) {
        return true; // Demo data already seeded; academic structure is idempotent
    }

    // Insert demo seed data
    std::string seedSql = R"(
        -- Users
        INSERT INTO users (username, password_hash, role, linked_student_id) VALUES
        ('admin', 'admin', 'admin', NULL),
        ('NCTU-2024-ICT-001', 'pass123', 'student', 1),
        ('NCTU-2024-ICT-002', 'pass123', 'student', 2),
        ('NCTU-2023-ICT-003', 'pass123', 'student', 3),
        ('NCTU-2023-MECT-004', 'pass123', 'student', 4),
        ('NCTU-2022-AUT-005', 'pass123', 'student', 5),
        ('NCTU-2022-RENEW-006', 'pass123', 'student', 6),
        ('NCTU-2021-PROS-007', 'pass123', 'student', 7),
        ('NCTU-2021-PETRO-008', 'pass123', 'student', 8);

        -- Students (with complete data)
        INSERT INTO students (student_code, first_name, middle_name, last_name, national_id, birth_date, nationality, gender, address, phone, email, college, program, level, entry_type, enrollment_date, academic_status, credits_completed, cgpa) VALUES
        ('NCTU-2024-ICT-001', 'Ahmed', 'Mohamed', 'Hassan', '29811234567890', '2003-05-15', 'Egyptian', 'male', 'Cairo, Nasr City, 15 Ahmed Fakhry St.', '+201234567890', 'ahmed.hassan@students.nctu.edu.eg', 'كلية تكنولوجيا الصناعة والطاقة', 'ICT', 2, 'fresh', '2024-09-01', 'active', 48, 3.2),
        ('NCTU-2024-ICT-002', 'Sara', 'Ahmed', 'Mohamed', '29911234567891', '2003-08-20', 'Egyptian', 'female', 'Giza, Dokki, 22 Tahrir Square', '+201234567891', 'sara.mohamed@students.nctu.edu.eg', 'كلية تكنولوجيا الصناعة والطاقة', 'ICT', 2, 'fresh', '2024-09-01', 'active', 48, 3.5),
        ('NCTU-2023-ICT-003', 'Youssef', '', 'Khaled', '29801011234567', '2003-01-10', 'Egyptian', 'male', 'Cairo, Maadi, 10 Nile St.', '+201234567893', 'youssef.khaled@students.nctu.edu.eg', 'كلية تكنولوجيا الصناعة والطاقة', 'ICT', 3, 'fresh', '2023-09-01', 'active', 72, 3.4),
        ('NCTU-2023-MECT-004', 'Mariam', 'Hany', 'Saad', '29903021234568', '2004-03-15', 'Egyptian', 'female', 'Giza, 6th October, 12 Central Axis', '+201234567894', 'mariam.saad@students.nctu.edu.eg', 'كلية تكنولوجيا الصناعة والطاقة', 'Mechatronics', 2, 'fresh', '2023-09-01', 'active', 36, 3.1),
        ('NCTU-2022-AUT-005', 'Ali', '', 'Mostafa', '29707051234569', '2002-07-05', 'Egyptian', 'male', 'Alexandria, Miami, 5 Sea View', '+201234567895', 'ali.mostafa@students.nctu.edu.eg', 'كلية تكنولوجيا الصناعة والطاقة', 'Autotronics', 3, 'fresh', '2022-09-01', 'active', 66, 3.0),
        ('NCTU-2022-RENEW-006', 'Nour', '', 'Ibrahim', '29711221234560', '2002-11-22', 'Egyptian', 'female', 'Mansoura, City Center, 7 University St.', '+201234567896', 'nour.ibrahim@students.nctu.edu.eg', 'كلية تكنولوجيا الصناعة والطاقة', 'Renewable Energy', 3, 'fresh', '2022-09-01', 'active', 60, 3.3),
        ('NCTU-2021-PROS-007', 'Hassan', '', 'Mahmoud', '29605011234561', '2001-05-01', 'Egyptian', 'male', 'Tanta, El Galaa St., 3', '+201234567897', 'hassan.mahmoud@students.nctu.edu.eg', 'كلية تكنولوجيا الصناعة والطاقة', 'Prosthetics & Orthotics', 4, 'fresh', '2021-09-01', 'active', 90, 3.6),
        ('NCTU-2021-PETRO-008', 'Salma', 'Omar', 'Fathy', '29609091234562', '2001-09-09', 'Egyptian', 'female', 'Suez, Port Area, 2 Canal St.', '+201234567898', 'salma.fathy@students.nctu.edu.eg', 'كلية تكنولوجيا الصناعة والطاقة', 'Petroleum Technology', 4, 'fresh', '2021-09-01', 'active', 88, 3.2);

        -- Enrollments (with assignment 1 & 2 marks)
        -- These use the seeded courses and model previous years + current term.
        INSERT INTO enrollments (student_id, course_id, semester, year, assignment1, assignment2, grade) VALUES
        -- Ahmed (ICT, Level 2) - previous year Level 1 terms
        (1, (SELECT id FROM courses WHERE course_code = 'MATH101'), 'Fall', 2023, 16.0, 22.0, 81.0),
        (1, (SELECT id FROM courses WHERE course_code = 'PHYS101'), 'Fall', 2023, 18.0, 21.0, 79.5),
        (1, (SELECT id FROM courses WHERE course_code = 'CS101'),   'Spring', 2024, 19.0, 24.0, 88.0),
        (1, (SELECT id FROM courses WHERE course_code = 'ENG101'),  'Spring', 2024, 17.0, 23.0, 84.5),

        -- Ahmed - current year Level 2 term
        (1, (SELECT id FROM courses WHERE course_code = 'DB201'),   'Fall', 2024, 20.0, 25.0, 89.5),
        (1, (SELECT id FROM courses WHERE course_code = 'NET101'),  'Fall', 2024, 18.0, 26.0, 91.0),

        -- Sara (ICT, Level 2) - previous year and current
        (2, (SELECT id FROM courses WHERE course_code = 'MATH101'), 'Fall', 2023, 17.0, 21.0, 83.0),
        (2, (SELECT id FROM courses WHERE course_code = 'CS101'),   'Spring', 2024, 19.0, 25.0, 90.0),
        (2, (SELECT id FROM courses WHERE course_code = 'DB201'),   'Fall', 2024, 18.0, 27.0, 92.5),

        -- Youssef (ICT, Level 3) - older history
        (3, (SELECT id FROM courses WHERE course_code = 'MATH101'), 'Fall', 2022, 15.0, 20.0, 76.0),
        (3, (SELECT id FROM courses WHERE course_code = 'CS101'),   'Spring', 2023, 18.0, 24.0, 85.0),
        (3, (SELECT id FROM courses WHERE course_code = 'NET101'),  'Fall', 2023, 19.0, 26.0, 89.0),
        (3, (SELECT id FROM courses WHERE course_code = 'DB201'),   'Spring', 2024, 20.0, 25.0, 93.0),

        -- Mariam (MECT, Level 2)
        (4, (SELECT id FROM courses WHERE course_code = 'MATH101'),    'Fall', 2023, 14.0, 20.0, 75.0),
        (4, (SELECT id FROM courses WHERE course_code = 'PHYS101'),    'Fall', 2023, 16.0, 21.0, 78.0),
        (4, (SELECT id FROM courses WHERE course_code = 'ELEC101'),    'Spring', 2024, 18.5, 23.0, 86.0),
        (4, (SELECT id FROM courses WHERE course_code = 'MECH201'),    'Fall', 2024, 17.0, 24.0, 82.5),

        -- Ali (Autotronics, Level 3)
        (5, (SELECT id FROM courses WHERE course_code = 'AUTO201'),    'Fall', 2022, 16.0, 22.0, 80.0),
        (5, (SELECT id FROM courses WHERE course_code = 'AUTO202'),    'Spring', 2023, 17.0, 23.0, 82.0),
        (5, (SELECT id FROM courses WHERE course_code = 'ELEC_AUTO201'),'Fall', 2023, 18.0, 24.0, 86.5),

        -- Nour (Renewable Energy, Level 3)
        (6, (SELECT id FROM courses WHERE course_code = 'MATH101'),    'Fall', 2022, 15.0, 20.0, 78.0),
        (6, (SELECT id FROM courses WHERE course_code = 'ENERG201'),   'Spring', 2023, 18.0, 24.0, 88.0),
        (6, (SELECT id FROM courses WHERE course_code = 'SOLAR301'),   'Fall', 2023, 19.0, 25.0, 90.0),

        -- Hassan (Prosthetics & Orthotics, Level 4)
        (7, (SELECT id FROM courses WHERE course_code = 'ANAT101'),    'Fall', 2021, 16.0, 21.0, 81.0),
        (7, (SELECT id FROM courses WHERE course_code = 'BIO201'),     'Spring', 2022, 17.5, 23.0, 84.0),
        (7, (SELECT id FROM courses WHERE course_code = 'PROS301'),    'Fall', 2022, 18.0, 24.0, 88.5),

        -- Salma (Petroleum, Level 4)
        (8, (SELECT id FROM courses WHERE course_code = 'CHEM101'),    'Fall', 2021, 15.0, 20.0, 77.0),
        (8, (SELECT id FROM courses WHERE course_code = 'GEO201'),     'Spring', 2022, 17.0, 22.0, 82.0),
        (8, (SELECT id FROM courses WHERE course_code = 'DRILL301'),   'Fall', 2022, 18.0, 24.0, 87.0);

        -- Attendance (sample records)
        INSERT INTO attendance (enrollment_id, date, status, remark) VALUES
        (1, '2023-10-15', 'present', ''),
        (1, '2023-10-22', 'present', ''),
        (2, '2023-10-18', 'present', ''),
        (3, '2024-03-10', 'absent', 'Sick'),
        (4, '2024-03-17', 'present', ''),
        (5, '2024-10-05', 'present', ''),
        (6, '2024-10-12', 'present', ''),
        (7, '2023-10-20', 'present', ''),
        (8, '2023-10-25', 'present', ''),
        (9, '2022-11-10', 'present', ''),
        (10, '2023-03-05', 'present', '');

        -- Financial Records
        INSERT INTO financial_records (student_id, record_type, amount, semester, year, payment_date, status, notes) VALUES
        (1, 'tuition', 5000.0, 'Fall',   2023, '2023-09-05', 'paid', 'Tuition fee for Fall 2023'),
        (1, 'tuition', 5000.0, 'Spring', 2024, '2024-02-20', 'paid', 'Tuition fee for Spring 2024'),
        (2, 'tuition', 5000.0, 'Fall',   2023, '2023-09-08', 'paid', 'Tuition fee for Fall 2023'),
        (3, 'tuition', 5000.0, 'Fall',   2022, '2022-09-10', 'paid', 'Tuition fee for Fall 2022'),
        (4, 'tuition', 5000.0, 'Fall',   2023, '2023-09-12', 'paid', 'Tuition fee for Fall 2023'),
        (5, 'tuition', 5000.0, 'Fall',   2022, '2022-09-15', 'paid', 'Tuition fee for Fall 2022'),
        (6, 'tuition', 5000.0, 'Fall',   2022, '2022-09-18', 'paid', 'Tuition fee for Fall 2022'),
        (7, 'tuition', 5000.0, 'Fall',   2021, '2021-09-20', 'paid', 'Tuition fee for Fall 2021'),
        (8, 'tuition', 5000.0, 'Fall',   2021, '2021-09-22', 'paid', 'Tuition fee for Fall 2021');

        -- Training Records
        INSERT INTO training_records (student_id, company_name, start_date, end_date, supervisor_name, supervisor_email, status, grade, evaluation) VALUES
        (3, 'Egyptian Engineering Company', '2024-06-01', '2024-08-31', 'Eng. Mohamed Ibrahim', 'm.ibrahim@eec.com', 'completed', 95.0, 'Excellent performance and dedication'),
        (1, 'Tech Solutions Egypt', '2025-06-01', NULL, 'Dr. Ahmed Fawzy', 'a.fawzy@techsol.com', 'ongoing', NULL, 'Summer internship'),
        (5, 'AutoTech Service Center', '2023-07-01', '2023-09-01', 'Eng. Tarek Ali', 't.ali@autotech.com', 'completed', 88.0, 'Good practical skills'),
        (6, 'Green Energy Co.', '2023-07-15', '2023-09-15', 'Eng. Rania Sami', 'r.sami@greenegy.com', 'completed', 90.0, 'Strong understanding of renewable systems');

        -- PMD sample records (per enrollment)
        INSERT INTO pmd_records (enrollment_id, pmd_type, doc_url, approved_by, approved_at) VALUES
        (3,  'MEDICAL',  'docs/pmd/ahmed_math101_medical.pdf',    1, '2024-03-20'),
        (8,  'DEFERRED', 'docs/pmd/sara_cs101_deferred.pdf',      1, '2024-04-10'),
        (13, 'MEDICAL',  'docs/pmd/youssef_net101_medical.pdf',   1, '2023-11-05'),
        (20, 'PASS',     'docs/pmd/ali_auto201_pass.pdf',         1, '2023-02-18');

    )";

    return executeNonQuery(seedSql);
}

