#ifndef NOTIFICATION_HPP
#define NOTIFICATION_HPP

#include <string>

struct Notification {
    int id;
    int userId;
    std::string title;
    std::string message;
    bool isRead;
    std::string createdAt;
};

#endif // NOTIFICATION_HPP