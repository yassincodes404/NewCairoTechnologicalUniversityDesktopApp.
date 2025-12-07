#ifndef FACULTY_HPP
#define FACULTY_HPP

#include <string>

struct Faculty {
    int id;
    std::string employeeCode;
    std::string title;
    std::string firstName;
    std::string lastName;
    std::string email;
    std::string phone;
    std::string department;
    std::string degrees;
};

#endif // FACULTY_HPP