#pragma once

#include "database.hpp"
#include "databaseconnectionpool.hpp"
#include <jsoncons/json.hpp>

using json = jsoncons::json;

class DatabaseHandler {
public:
    DatabaseHandler(DatabaseConnectionPool& dbConnPool);
    ~DatabaseHandler() = default;
    json executeQuery(const std::string& query);
    json executeReadQuery(const std::string& query);

private:
    DatabaseConnectionPool& databaseConnectionPool;
};
