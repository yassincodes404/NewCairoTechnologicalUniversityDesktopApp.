#include "App.hpp"
#include "../ui/screens/LoginScreen.hpp"
#include "../ui/screens/MainMenuScreen.hpp"
#include "../ui/screens/StudentListScreen.hpp"
#include "../ui/screens/StudentDetailsScreen.hpp"
#include "../ui/screens/CourseListScreen.hpp"
#include "../ui/screens/ProgramViewScreen.hpp"
#include <iostream>

App::App()
    : database(),
      studentRepository(database),
      courseRepository(database),
      enrollmentRepository(database),
      programRepository(database),
      authService(database),
      studentService(studentRepository, enrollmentRepository),
      courseService(courseRepository),
      programService(programRepository),
      currentScreen(ScreenType::Login),
      activeScreen(nullptr) {
}

App::~App() {
    database.close();
}

bool App::init() {
    // Open database - try multiple paths
    std::string dbPath = "data/nctu.db";
    
    // If running from build directory, try parent directory first
    if (!database.open(dbPath)) {
        dbPath = "../data/nctu.db";
        if (!database.open(dbPath)) {
            // Try creating data directory in current location
            system("mkdir -p data 2>/dev/null");
            dbPath = "data/nctu.db";
            if (!database.open(dbPath)) {
                std::cerr << "Failed to open database: " << database.getLastError() << std::endl;
                std::cerr << "Tried paths: data/nctu.db, ../data/nctu.db" << std::endl;
                return false;
            }
        }
    }

    // Initialize schema
    if (!database.initialize()) {
        std::cerr << "Failed to initialize database: " << database.getLastError() << std::endl;
        return false;
    }

    // Seed data
    if (!database.seedData()) {
        std::cerr << "Failed to seed data: " << database.getLastError() << std::endl;
        // Continue anyway, might already be seeded
    }

    // Start with login screen
    switchScreen(ScreenType::Login);

    return true;
}

void App::handleEvent() {
    if (activeScreen) {
        activeScreen->handleEvent();
    }
}

void App::update(float dt) {
    if (activeScreen) {
        activeScreen->update(dt);
    }
}

void App::draw() {
    if (activeScreen) {
        activeScreen->draw();
    }
}

void App::switchScreen(ScreenType type) {
    currentScreen = type;
    activeScreen.reset();

    switch (type) {
        case ScreenType::Login:
            activeScreen = std::make_unique<LoginScreen>(*this);
            break;
        case ScreenType::MainMenu:
            activeScreen = std::make_unique<MainMenuScreen>(*this);
            break;
        case ScreenType::StudentList:
            activeScreen = std::make_unique<StudentListScreen>(*this);
            break;
        case ScreenType::StudentDetails:
            activeScreen = std::make_unique<StudentDetailsScreen>(*this);
            break;
        case ScreenType::CourseList:
            activeScreen = std::make_unique<CourseListScreen>(*this);
            break;
        case ScreenType::ProgramView:
            activeScreen = std::make_unique<ProgramViewScreen>(*this);
            break;
        case ScreenType::EnrollmentList:
            // TODO: Implement EnrollmentListScreen
            break;
    }
}

