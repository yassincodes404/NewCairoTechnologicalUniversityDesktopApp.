#include "LoginScreen.hpp"
#include "../../app/App.hpp"
#include "../ScreenType.hpp"
#include <iostream>

LoginScreen::LoginScreen(App& app)
    : app(app),
      usernameInput(400, 250, 300, 40),
      passwordInput(400, 310, 300, 40),
      loginButton(400, 380, 300, 50, "Login"),
      errorMessage("") {
    passwordInput.setIsPassword(true);
}

void LoginScreen::handleEvent() {
    usernameInput.handleEvent();
    passwordInput.handleEvent();
    loginButton.update();

    if (loginButton.isClicked() || IsKeyPressed(KEY_ENTER)) {
        std::string username = usernameInput.getText();
        std::string password = passwordInput.getText();

        if (username.empty() || password.empty()) {
            errorMessage = "Please enter username and password";
            return;
        }

        User user;
        if (app.authService.login(username, password, user)) {
            app.currentUser = user;
            errorMessage = "";

            // If student, go directly to their details
            if (user.role == "student" && user.linkedStudentId.has_value()) {
                auto student = app.studentRepository.getById(user.linkedStudentId.value());
                if (student.has_value()) {
                    app.selectedStudent = student.value();
                    app.switchScreen(ScreenType::StudentDetails);
                } else {
                    app.switchScreen(ScreenType::MainMenu);
                }
            } else {
                app.switchScreen(ScreenType::MainMenu);
            }
        } else {
            errorMessage = "Invalid username or password";
        }
    }
}

void LoginScreen::update(float dt) {
    // Update animations if needed
}

void LoginScreen::draw() {
    // Title
    const char* title = "NCTU Student Information System";
    Vector2 titleSize = MeasureTextEx(GetFontDefault(), title, 36, 2);
    DrawTextEx(GetFontDefault(), title, 
               {(float)GetScreenWidth() / 2 - titleSize.x / 2, 150}, 
               36, 2, WHITE);

    // Username label
    DrawText("Username:", 400, 230, 18, LIGHTGRAY);
    usernameInput.draw();

    // Password label
    DrawText("Password:", 400, 290, 18, LIGHTGRAY);
    passwordInput.draw();

    // Login button
    loginButton.draw();

    // Error message
    if (!errorMessage.empty()) {
        DrawText(errorMessage.c_str(), 400, 450, 18, RED);
    }

    // Hint
    DrawText("Try: admin/admin or NCTU-2024-ICT-001/pass123", 400, 500, 16, DARKGRAY);
}

