#include "StudentDetailsScreen.hpp"
#include "../ScreenType.hpp"
#include <sstream>
#include <iomanip>

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
    DrawTextEx(GetFontDefault(), title, {250, 100}, 32, 2, WHITE);

    float y = 160.0f;
    
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

    // Enrollments section
    y += 20;
    DrawTextEx(GetFontDefault(), "Enrolled Courses:", {250.0f, y}, 24, 2, WHITE);
    y += 40;

    if (enrollments.empty()) {
        DrawText("No enrollments found", 250, (int)y, 18, GRAY);
    } else {
        // Header
        DrawText("Course Code", 250, (int)y, 18, LIGHTGRAY);
        DrawText("Title", 400, (int)y, 18, LIGHTGRAY);
        DrawText("Semester", 650, (int)y, 18, LIGHTGRAY);
        DrawText("Year", 750, (int)y, 18, LIGHTGRAY);
        DrawText("Grade", 800, (int)y, 18, LIGHTGRAY);
        y += 30;

        // Draw line
        DrawLine(250, (int)y - 5, 900, (int)y - 5, GRAY);

        // Rows
        for (const auto& [course, enrollment] : enrollments) {
            DrawText(course.courseCode.c_str(), 250, (int)y, 18, WHITE);
            DrawText(course.title.c_str(), 400, (int)y, 18, WHITE);
            DrawText(enrollment.semester.c_str(), 650, (int)y, 18, WHITE);
            DrawText(std::to_string(enrollment.year).c_str(), 750, (int)y, 18, WHITE);
            
            if (enrollment.grade.has_value()) {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(1) << enrollment.grade.value();
                DrawText(ss.str().c_str(), 800, (int)y, 18, GREEN);
            } else {
                DrawText("N/A", 800, (int)y, 18, GRAY);
            }
            y += 30;
        }
    }
}

