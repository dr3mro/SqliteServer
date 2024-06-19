#include "databaseconnectionpool.hpp"
#include <iostream>

// Define constants for database connection
const char* DB_HOST = "172.17.0.3";
const char* DB_NAME = "postgres";
const char* DB_USER = "postgres";
const char* DB_PASSWORD = "000";

DatabaseConnectionPool::DatabaseConnectionPool(size_t pool_size)
    : pool_size(pool_size)
    , available_connections(pool_size)
{
    for (size_t i = 0; i < pool_size; ++i) {
        auto conn = std::make_shared<pqxx::connection>(
            "host=" + std::string(DB_HOST) + " dbname=" + std::string(DB_NAME) + " user=" + std::string(DB_USER) + " password=" + std::string(DB_PASSWORD));
        if (conn->is_open()) {
            auto database = std::make_shared<Database>(conn);
            connections.push(database);
        } else {
            std::cerr << "Failed to open database connection" << std::endl;
        }
    }
}

// Get a connection from the pool
std::shared_ptr<Database> DatabaseConnectionPool::get_connection()
{
    std::unique_lock<std::mutex> lock(mutex);
    while (connections.empty()) {
        cv.wait(lock);
    }
    auto db = connections.front();
    connections.pop();
    return db;
}

// Return a connection to the pool
void DatabaseConnectionPool::return_connection(std::shared_ptr<Database> db)
{
    std::lock_guard<std::mutex> lock(mutex);
    connections.push(db);
    cv.notify_one();
}
