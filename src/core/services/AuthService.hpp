#ifndef AUTH_SERVICE_HPP
#define AUTH_SERVICE_HPP

#include "../models/User.hpp"
#include "../Database.hpp"
#include <string>
#include <optional>

class AuthService {
public:
    explicit AuthService(Database& db) : database(db) {}

    bool login(const std::string& username, const std::string& password, User& outUser);
    std::optional<User> getUserByUsername(const std::string& username);

private:
    Database& database;
    User rowToUser(int argc, char** argv, char** colNames);
};

#endif // AUTH_SERVICE_HPP

