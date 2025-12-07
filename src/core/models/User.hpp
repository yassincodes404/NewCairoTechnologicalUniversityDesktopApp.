#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <optional>

struct User {
    int id;
    std::string username;
    std::string role; // "student" or "admin"
    std::optional<int> linkedStudentId; // if role is "student"
};

#endif // USER_HPP

