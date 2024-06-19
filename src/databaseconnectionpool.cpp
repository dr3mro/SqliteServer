#include "databaseconnectionpool.hpp"
#include <iostream>
#include <pqxx/pqxx> // Assuming pqxx library is used

// Define constants for database connection
const char* DB_HOST = "172.17.0.2";
const char* DB_NAME = "postgres";
const char* DB_USER = "postgres";
const char* DB_PASSWORD = "000";

DatabaseConnectionPool::DatabaseConnectionPool(size_t pool_size)
    : pool_size(pool_size)
{
    try {
        for (size_t i = 0; i < pool_size; ++i) {
            auto conn = std::make_shared<pqxx::connection>(
                "host=" + std::string(DB_HOST) + " dbname=" + std::string(DB_NAME) + " user=" + std::string(DB_USER) + " password=" + std::string(DB_PASSWORD));
            if (conn->is_open()) {
                auto database = std::make_shared<Database>(conn);
                databaseConnections.push(database);
                std::cout << "Database connection added to pool." << std::endl;
            } else {
                std::cerr << "Failed to open database connection" << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception caught during database connection pool initialization: " << e.what() << std::endl;
        throw; // Rethrow exception to indicate initialization failure
    }
}

// Get a connection from the pool
std::shared_ptr<Database> DatabaseConnectionPool::get_connection()
{
    std::unique_lock<std::mutex> lock(mutex);
    while (databaseConnections.empty()) {
        cv.wait(lock);
    }
    auto db = databaseConnections.front();
    databaseConnections.pop();
    std::cout << "Database connection retrieved from pool. Connections remaining: " << databaseConnections.size() << std::endl;
    return db;
}

// Return a connection to the pool
void DatabaseConnectionPool::return_connection(std::shared_ptr<Database> db)
{
    std::lock_guard<std::mutex> lock(mutex);
    databaseConnections.push(db);
    cv.notify_one();
    std::cout << "Database connection returned to pool. Connections available: " << databaseConnections.size() << std::endl;
}
