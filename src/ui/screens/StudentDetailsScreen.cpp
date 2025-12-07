#include "StudentDetailsScreen.hpp"
#include "../ScreenType.hpp"
#include <sstream>
#include <iomanip>
#include <map>

StudentDetailsScreen::StudentDetailsScreen(App& app)
    : app(app),
      backButton(50, 50, 100, 40, "Back") {
    
    if (app.selectedStudent.has_value()) {
        student = app.selectedStudent.value();
        enrollments = app.studentService.getStudentCoursesWithGrades(student.id);
    } else {
        // Fallback: try to get from current user
        if (app.currentUser.has_value() && app.currentUser->linkedStudentId.has_value()) {
            auto s = app.studentRepository.getById(app.currentUser->linkedStudentId.value());
            if (s.has_value()) {
                student = s.value();
                enrollments = app.studentService.getStudentCoursesWithGrades(student.id);
            }
        }
    }

    // Preload PMD flags for this student's enrollments
    PmdRepository pmdRepo(app.database);
    for (const auto& ce : enrollments) {
        const Enrollment& e = ce.second;
        auto records = pmdRepo.getByEnrollment(e.id);
        if (!records.empty()) {
            pmdEnrollmentIds.insert(e.id);
        }
    }
}

void StudentDetailsScreen::handleEvent() {
    backButton.update();

    if (backButton.isClicked()) {
        if (app.currentUser.has_value() && app.currentUser->role == "student") {
            app.switchScreen(ScreenType::MainMenu);
        } else {
            app.switchScreen(ScreenType::StudentList);
        }
    }
}

void StudentDetailsScreen::update(float dt) {
    // Update animations if needed
}

void StudentDetailsScreen::draw() {
    backButton.draw();

    // Student info section
    const char* title = "Student Details";
    DrawTextEx(GetFontDefault(), title, {250, 70}, 32, 2, WHITE);

    // Profile picture placeholder (circle with initials)
    float avatarX = 80.0f;
    float avatarY = 120.0f;
    float avatarRadius = 40.0f;
    DrawCircle((int)avatarX, (int)avatarY, avatarRadius, (Color){60, 80, 120, 255});
    DrawCircleLines((int)avatarX, (int)avatarY, avatarRadius, (Color){150, 180, 255, 255});

    std::string initials;
    if (!student.firstName.empty()) {
        initials += (char)std::toupper(student.firstName[0]);
    }
    if (!student.lastName.empty()) {
        initials += (char)std::toupper(student.lastName[0]);
    }
    if (!initials.empty()) {
        int fontSize = 28;
        int textWidth = MeasureText(initials.c_str(), fontSize);
        DrawText(initials.c_str(),
                 (int)(avatarX - textWidth / 2),
                 (int)(avatarY - fontSize / 2),
                 fontSize,
                 WHITE);
    }

    float y = 120.0f;
    
    // Personal Information Section
    DrawTextEx(GetFontDefault(), "Personal Information", {250.0f, y}, 22, 2, (Color){100, 150, 255, 255});
    y += 35;
    
    DrawText(("Student Code: " + student.studentCode).c_str(), 250, (int)y, 18, WHITE);
    y += 25;
    
    std::string fullName = student.firstName;
    if (!student.middleName.empty()) fullName += " " + student.middleName;
    fullName += " " + student.lastName;
    DrawText(("Name: " + fullName).c_str(), 250, (int)y, 18, WHITE);
    y += 25;
    
    if (!student.nationalId.empty()) {
        DrawText(("National ID: " + student.nationalId).c_str(), 250, (int)y, 18, LIGHTGRAY);
        y += 25;
    }
    if (!student.birthDate.empty()) {
        DrawText(("Birth Date: " + student.birthDate).c_str(), 250, (int)y, 18, LIGHTGRAY);
        y += 25;
    }
    if (!student.gender.empty()) {
        DrawText(("Gender: " + student.gender).c_str(), 250, (int)y, 18, LIGHTGRAY);
        y += 25;
    }
    if (!student.nationality.empty()) {
        DrawText(("Nationality: " + student.nationality).c_str(), 250, (int)y, 18, LIGHTGRAY);
        y += 25;
    }
    
    y += 10;
    
    // Contact Information Section
    DrawTextEx(GetFontDefault(), "Contact Information", {250.0f, y}, 22, 2, (Color){100, 150, 255, 255});
    y += 35;
    
    if (!student.address.empty()) {
        DrawText(("Address: " + student.address).c_str(), 250, (int)y, 18, WHITE);
        y += 25;
    }
    if (!student.phone.empty()) {
        DrawText(("Phone: " + student.phone).c_str(), 250, (int)y, 18, WHITE);
        y += 25;
    }
    if (!student.email.empty()) {
        DrawText(("Email: " + student.email).c_str(), 250, (int)y, 18, WHITE);
        y += 25;
    }
    
    y += 10;
    
    // Academic Information Section
    DrawTextEx(GetFontDefault(), "Academic Information", {250.0f, y}, 22, 2, (Color){100, 150, 255, 255});
    y += 35;
    
    if (!student.college.empty()) {
        DrawText(("College: " + student.college).c_str(), 250, (int)y, 18, WHITE);
        y += 25;
    }
    DrawText(("Program: " + student.program).c_str(), 250, (int)y, 18, WHITE);
    y += 25;
    DrawText(("Level: " + std::to_string(student.level)).c_str(), 250, (int)y, 18, WHITE);
    y += 25;

    // Level structure overview (Foundation/Core/Professional/Graduation)
    std::string levelStage;
    if (student.level == 1) {
        levelStage = "Stage: Foundation Level (Level 1)";
    } else if (student.level == 2) {
        levelStage = "Stage: Core Technical Level (Level 2)";
    } else if (student.level == 3) {
        levelStage = "Stage: Professional / Advanced Level (Level 3)";
    } else if (student.level == 4) {
        levelStage = "Stage: Graduation Level (Level 4)";
    }
    if (!levelStage.empty()) {
        DrawText(levelStage.c_str(), 250, (int)y, 18, LIGHTGRAY);
        y += 25;
    }

    if (!student.entryType.empty()) {
        DrawText(("Entry Type: " + student.entryType).c_str(), 250, (int)y, 18, LIGHTGRAY);
        y += 25;
    }
    if (!student.academicStatus.empty()) {
        Color statusColor = (student.academicStatus == "active") ? GREEN : 
                           (student.academicStatus == "probation") ? YELLOW : RED;
        DrawText(("Status: " + student.academicStatus).c_str(), 250, (int)y, 18, statusColor);
        y += 25;
    }
    if (!student.enrollmentDate.empty()) {
        DrawText(("Enrollment Date: " + student.enrollmentDate).c_str(), 250, (int)y, 18, LIGHTGRAY);
        y += 25;
    }
    
    // Calculate CGPA if we have grades
    if (!enrollments.empty()) {
        float totalPoints = 0.0f;
        int totalCredits = 0;
        int passedCredits = 0;
        
        for (const auto& [course, enrollment] : enrollments) {
            if (enrollment.grade.has_value()) {
                float grade = enrollment.grade.value();
                // Simple GPA calculation (A=4.0, B=3.0, etc.)
                float points = 0.0f;
                if (grade >= 90) points = 4.0f;
                else if (grade >= 80) points = 3.0f;
                else if (grade >= 70) points = 2.0f;
                else if (grade >= 60) points = 1.0f;
                
                totalPoints += points * course.credits;
                totalCredits += course.credits;
                if (grade >= 60) passedCredits += course.credits;
            }
        }
        
        if (totalCredits > 0) {
            float cgpa = totalPoints / totalCredits;
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << "CGPA: " << cgpa;
            DrawText(ss.str().c_str(), 250, (int)y, 20, (Color){100, 255, 150, 255});
            y += 30;
            
            ss.str("");
            ss << "Total Credits: " << totalCredits << " (Passed: " << passedCredits << ")";
            DrawText(ss.str().c_str(), 250, (int)y, 18, LIGHTGRAY);
            y += 30;
        }
    }

    // Group enrollments by academic term (Year + Semester)
    if (!enrollments.empty()) {
        y += 20;
        DrawTextEx(GetFontDefault(), "Academic Record by Term:", {250.0f, y}, 24, 2, WHITE);
        y += 40;

        // Build groups
        std::map<std::string, std::vector<std::pair<Course, Enrollment>>> termGroups;
        for (const auto& ce : enrollments) {
            const Course& course = ce.first;
            const Enrollment& enrollment = ce.second;
            std::stringstream key;
            key << enrollment.year << " - " << enrollment.semester;
            termGroups[key.str()].push_back(ce);
        }

        for (const auto& entry : termGroups) {
            const std::string& termLabel = entry.first;
            const auto& termEnrollments = entry.second;

            // Term header
            DrawText(termLabel.c_str(), 250, (int)y, 20, (Color){200, 200, 255, 255});

            // Compute term GPA
            float termPoints = 0.0f;
            int termCredits = 0;
            for (const auto& ce : termEnrollments) {
                const Course& course = ce.first;
                const Enrollment& enrollment = ce.second;
                if (enrollment.grade.has_value()) {
                    float grade = enrollment.grade.value();
                    float points = 0.0f;
                    if (grade >= 90) points = 4.0f;
                    else if (grade >= 80) points = 3.0f;
                    else if (grade >= 70) points = 2.0f;
                    else if (grade >= 60) points = 1.0f;
                    termPoints += points * course.credits;
                    termCredits += course.credits;
                }
            }

            if (termCredits > 0) {
                float termGpa = termPoints / termCredits;
                std::stringstream ss;
                ss << std::fixed << std::setprecision(2) << "Term GPA: " << termGpa
                   << "  | Credits: " << termCredits;
                DrawText(ss.str().c_str(), 450, (int)y, 18, LIGHTGRAY);
            }

            y += 30;

            // Header row
            DrawText("Course Code", 250, (int)y, 18, LIGHTGRAY);
            DrawText("Title",       380, (int)y, 18, LIGHTGRAY);
            DrawText("A1",         640, (int)y, 18, LIGHTGRAY);
            DrawText("A2",         690, (int)y, 18, LIGHTGRAY);
            DrawText("Final",      740, (int)y, 18, LIGHTGRAY);
            y += 25;

            DrawLine(250, (int)y - 5, 880, (int)y - 5, GRAY);

            // Rows for this term
            for (const auto& ce : termEnrollments) {
                const Course& course = ce.first;
                const Enrollment& enrollment = ce.second;

                DrawText(course.courseCode.c_str(), 250, (int)y, 18, WHITE);
                DrawText(course.title.c_str(),       380, (int)y, 18, WHITE);

                // Assignment 1
                if (enrollment.assignment1.has_value()) {
                    std::stringstream ssA1;
                    ssA1 << std::fixed << std::setprecision(1) << enrollment.assignment1.value();
                    DrawText(ssA1.str().c_str(), 640, (int)y, 18, LIGHTGRAY);
                } else {
                    DrawText("-", 640, (int)y, 18, GRAY);
                }

                // Assignment 2
                if (enrollment.assignment2.has_value()) {
                    std::stringstream ssA2;
                    ssA2 << std::fixed << std::setprecision(1) << enrollment.assignment2.value();
                    DrawText(ssA2.str().c_str(), 690, (int)y, 18, LIGHTGRAY);
                } else {
                    DrawText("-", 690, (int)y, 18, GRAY);
                }

                // Final grade
                if (enrollment.grade.has_value()) {
                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(1) << enrollment.grade.value();
                    DrawText(ss.str().c_str(), 740, (int)y, 18, GREEN);
                } else {
                    DrawText("N/A", 740, (int)y, 18, GRAY);
                }

                y += 25;
            }

            y += 20;
        }
    } else {
        // No enrollments at all
        y += 20;
        DrawText("No academic records found for this student.", 250, (int)y, 18, GRAY);
    }
}

