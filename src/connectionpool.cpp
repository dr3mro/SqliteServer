#include "connectionpool.hpp"

ConnectionPool::ConnectionPool(const std::string& dbFile, size_t poolSize)
{
    try {
        for (size_t i = 0; i < poolSize; ++i) {
            auto connection = std::make_shared<SQLite::Database>(dbFile, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
            SQLite::Statement query1(*connection, "PRAGMA journal_mode=WAL");
            query1.executeStep(); // Ensure the statement is executed
            SQLite::Statement query2(*connection, "pragma synchronous = off;");
            query2.executeStep(); // Ensure the statement is executed
            SQLite::Statement query3(*connection, "pragma temp_store = memory;");
            query3.executeStep(); // Ensure the statement is executed
            SQLite::Statement query4(*connection, "pragma mmap_size = 60000000000;");
            query4.executeStep(); // Ensure the statement is executed
            SQLite::Statement query5(*connection, "pragma page_size = 64000;");
            query5.executeStep(); // Ensure the statement is executed
            SQLite::Statement query6(*connection, "pragma busy_timeout = 10000;");
            query6.executeStep(); // Ensure the statement is executed
            pool.push(connection);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error initializing connection pool: " << e.what() << std::endl;
        // Handle cleanup if necessary
        throw;
    }
}

void ConnectionPool::returnConnection(std::shared_ptr<SQLite::Database> conn)
{
    std::unique_lock<std::mutex> lock(pool_mutex);
    pool.push(conn);
}

ConnectionPool::~ConnectionPool()
{
    std::cout << "Connections: " << pool.size() << std::endl;
    while (!pool.empty()) {
        auto c = pool.front();
        pool.pop();
        // Ensure proper cleanup if necessary
    }
}

std::shared_ptr<SQLite::Database> ConnectionPool::getConnection()
{
    std::unique_lock<std::mutex> lock(pool_mutex);
    if (pool.empty()) {
        throw std::runtime_error("No available connections.");
    }
    auto conn = pool.front();
    pool.pop();
    return conn;
}