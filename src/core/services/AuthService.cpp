#include "AuthService.hpp"
#include <sstream>

namespace {
std::string escapeSql(const std::string& value) {
    std::string escaped;
    escaped.reserve(value.size());
    for (char c : value) {
        escaped.push_back(c);
        if (c == '\'') {
            escaped.push_back('\'');
        }
    }
    return escaped;
}
}

bool AuthService::login(const std::string& username, const std::string& password, User& outUser) {
    std::stringstream sql;
    sql << "SELECT id, username, role, linked_student_id FROM users "
        << "WHERE username = '" << escapeSql(username) << "' "
        << "AND password_hash = '" << escapeSql(password) << "';";

    std::optional<User> user;
    database.executeQuery(sql.str(), [&user, this](void*, int argc, char** argv, char** colNames) -> int {
        user = rowToUser(argc, argv, colNames);
        return 0;
    });

    if (user.has_value()) {
        outUser = user.value();
        return true;
    }

    return false;
}

std::optional<User> AuthService::getUserByUsername(const std::string& username) {
    std::optional<User> result;
    std::stringstream sql;
    sql << "SELECT id, username, role, linked_student_id FROM users "
        << "WHERE username = '" << escapeSql(username) << "';";

    database.executeQuery(sql.str(), [&result, this](void*, int argc, char** argv, char** colNames) -> int {
        result = rowToUser(argc, argv, colNames);
        return 0;
    });

    return result;
}

User AuthService::rowToUser(int argc, char** argv, char** colNames) {
    User u;
    for (int i = 0; i < argc; i++) {
        std::string colName = colNames[i];
        std::string value = argv[i] ? argv[i] : "";

        if (colName == "id") u.id = std::stoi(value);
        else if (colName == "username") u.username = value;
        else if (colName == "role") u.role = value;
        else if (colName == "linked_student_id" && !value.empty()) {
            u.linkedStudentId = std::stoi(value);
        }
    }
    return u;
}

