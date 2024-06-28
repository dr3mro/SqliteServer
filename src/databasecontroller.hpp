#pragma once

#include "database.hpp"
#include "databaseconnectionpool.hpp"
#include <jsoncons/json.hpp>

using json = jsoncons::json;

class DatabaseController {
public:
    DatabaseController(DatabaseConnectionPool& dbConnPool);
    ~DatabaseController() = default;
    json executeQuery(const std::string& query);
    json executeReadQuery(const std::string& query);
    bool checkItemExists(const std::string& table, const std::string& column, const std::string& value);
    std::string getPasswordHashForUserID(const uint64_t& user_id);
    uint64_t findIfUserID(const std::string& username); // check if user found and return 0 if not found or return ID if found

private:
    DatabaseConnectionPool& databaseConnectionPool;
};
