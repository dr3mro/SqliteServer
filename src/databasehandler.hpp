#pragma once

#include "database.hpp"
#include "databaseconnectionpool.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class DatabaseHandler {
public:
    DatabaseHandler(DatabaseConnectionPool& dbConnPool);
    ~DatabaseHandler() = default;
    json executeQuery(const std::string& query);

private:
    DatabaseConnectionPool& databaseConnectionPool;
};
