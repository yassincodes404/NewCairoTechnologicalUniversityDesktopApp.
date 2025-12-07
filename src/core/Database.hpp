#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <sqlite3.h>
#include <string>
#include <functional>
#include <optional>

class Database {
public:
    Database();
    ~Database();

    bool open(const std::string& dbPath);
    void close();
    bool isOpen() const { return db != nullptr; }

    // Execute SQL without return value
    bool executeNonQuery(const std::string& sql);

    // Execute SQL with callback for each row
    bool executeQuery(const std::string& sql, 
                     std::function<int(void*, int, char**, char**)> callback,
                     void* userData = nullptr);

    // Get last error message
    std::string getLastError() const;

    // Initialize schema (create tables)
    bool initialize();

    // Seed initial data
    bool seedData();

    // Get raw sqlite3* handle (for advanced use)
    sqlite3* getHandle() { return db; }

private:
    sqlite3* db;
    std::string lastError;
};

#endif // DATABASE_HPP

