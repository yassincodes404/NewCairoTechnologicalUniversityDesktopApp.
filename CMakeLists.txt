cmake_minimum_required(VERSION 3.15)
project(NCTU_SIS_Client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find raylib
find_library(RAYLIB_LIB raylib PATHS /usr/lib /usr/local/lib)
if(NOT RAYLIB_LIB)
    message(FATAL_ERROR "raylib not found. Please install raylib development package.")
endif()

# Find SQLite3
find_library(SQLITE3_LIB sqlite3 PATHS /usr/lib /usr/local/lib)
if(NOT SQLITE3_LIB)
    message(FATAL_ERROR "sqlite3 not found. Please install sqlite3 development package.")
endif()

# Include directories
include_directories(src)

# Source files
set(SOURCES
    src/main.cpp
    src/app/App.cpp
    src/core/Database.cpp
    src/core/repositories/StudentRepository.cpp
    src/core/repositories/CourseRepository.cpp
    src/core/repositories/EnrollmentRepository.cpp
    src/core/services/AuthService.cpp
    src/core/services/StudentService.cpp
    src/core/services/CourseService.cpp
    src/ui/screens/LoginScreen.cpp
    src/ui/screens/MainMenuScreen.cpp
    src/ui/screens/StudentListScreen.cpp
    src/ui/screens/StudentDetailsScreen.cpp
    src/ui/screens/CourseListScreen.cpp
    src/ui/widgets/Button.cpp
    src/ui/widgets/TextInput.cpp
    src/ui/widgets/SimpleTable.cpp
)

# Create executable
add_executable(nctu_sis_client ${SOURCES})

# Link libraries
target_link_libraries(nctu_sis_client ${RAYLIB_LIB} ${SQLITE3_LIB} pthread dl rt X11 GL m)

# Copy assets and data directories to build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data)
# Note: Database will be created automatically on first run

