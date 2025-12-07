#ifndef APP_HPP
#define APP_HPP

#include "../ui/Screen.hpp"
#include "../ui/ScreenType.hpp"
#include "../core/Database.hpp"
#include "../core/models/User.hpp"
#include "../core/models/Student.hpp"
#include "../core/repositories/StudentRepository.hpp"
#include "../core/repositories/CourseRepository.hpp"
#include "../core/repositories/EnrollmentRepository.hpp"
#include "../core/services/AuthService.hpp"
#include "../core/services/StudentService.hpp"
#include "../core/services/CourseService.hpp"
#include <memory>
#include <optional>

class App {
public:
    App();
    ~App();

    bool init();
    void handleEvent();
    void update(float dt);
    void draw();

    void switchScreen(ScreenType type);

    // Shared state
    Database database;
    StudentRepository studentRepository;
    CourseRepository courseRepository;
    EnrollmentRepository enrollmentRepository;
    AuthService authService;
    StudentService studentService;
    CourseService courseService;

    std::optional<User> currentUser;
    std::optional<Student> selectedStudent;
    
    bool shouldQuit = false; // Flag to signal app should close

private:
    ScreenType currentScreen;
    std::unique_ptr<Screen> activeScreen;
};

#endif // APP_HPP

