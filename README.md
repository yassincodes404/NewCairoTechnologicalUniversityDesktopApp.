# NCTU SIS Client - Student Information System

A desktop application for New Cairo Technological University built with C++17, Raylib, and SQLite.

## Architecture

The project follows a clean architecture with clear separation of concerns:

```
src/
├── app/              # Application controller (App class)
├── core/             # Business logic (no UI dependencies)
│   ├── models/       # Data structures (Student, Course, Enrollment, User)
│   ├── repositories/ # Database access layer
│   └── services/    # Business logic services
└── ui/               # User interface (Raylib)
    ├── screens/     # Screen implementations
    └── widgets/     # Reusable UI components
```

## Building

### Using CMake

```bash
mkdir build
cd build
cmake ..
make
```

### Direct compilation (alternative)

```bash
g++ -std=c++17 -o nctu_sis_client \
    src/main.cpp \
    src/app/App.cpp \
    src/core/Database.cpp \
    src/core/repositories/*.cpp \
    src/core/services/*.cpp \
    src/ui/screens/*.cpp \
    src/ui/widgets/*.cpp \
    -lraylib -lsqlite3 -lpthread -ldl -lrt -lX11 -lGL -lm
```

## Running

```bash
cd build
./nctu_sis_client
```

## Features

### Current (v1.0)
- ✅ User authentication (login)
- ✅ Student list view
- ✅ Student details with enrollments
- ✅ Course enrollment display
- ✅ SQLite database integration
- ✅ Screen-based navigation

### Database

The application uses SQLite with the following schema:
- `users` - Authentication
- `students` - Student records
- `courses` - Course catalog
- `enrollments` - Student-course relationships with grades

Database file: `data/nctu.db` (created automatically on first run)

### Default Credentials

- **Admin**: `admin` / `admin`
- **Student 1**: `NCTU-2024-ICT-001` / `pass123`
- **Student 2**: `NCTU-2024-ICT-002` / `pass123`

## Project Structure

### Core Layer (No UI dependencies)
- `Database` - SQLite wrapper
- `StudentRepository`, `CourseRepository`, `EnrollmentRepository` - Data access
- `AuthService`, `StudentService` - Business logic

### UI Layer
- `Screen` - Base interface for all screens
- `LoginScreen` - User authentication
- `MainMenuScreen` - Navigation menu
- `StudentListScreen` - Browse students
- `StudentDetailsScreen` - View student info and enrollments
- `Button`, `TextInput` - Reusable widgets

### App Controller
- Manages screen transitions
- Holds shared state (current user, selected student)
- Initializes database and services

## Navigation Flow

1. **Login** → Enter credentials
2. **Main Menu** (admin) or **Student Details** (student)
3. **Student List** → Browse all students
4. **Student Details** → View student info and courses

## Future Enhancements

- [ ] Course list screen
- [ ] Enrollment management
- [ ] Grade entry interface
- [ ] Attendance tracking
- [ ] PMD (Pass/Medical/Deferred) system
- [ ] Rafeh (Mercy) grade adjustments
- [ ] Arabic language support
- [ ] Advanced UI styling
- [ ] PDF transcript generation

## Dependencies

- **Raylib** - Graphics and windowing
- **SQLite3** - Database
- **C++17** - Language standard

## License

Internal use - New Cairo Technological University

