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

private:
    DatabaseConnectionPool& databaseConnectionPool;
};
