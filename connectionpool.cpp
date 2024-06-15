#include "connectionpool.hpp"
ConnectionPool::ConnectionPool(const std::string& dbFile, size_t poolSize)
{
    for (size_t i = 0; i < poolSize; ++i) {
        pool.push(std::make_shared<SQLite::Database>(dbFile, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE));
    }
}
void ConnectionPool::returnConnection(std::shared_ptr<SQLite::Database> conn)
{
    std::unique_lock<std::mutex> lock(pool_mutex);
    pool.push(conn);
}

ConnectionPool::~ConnectionPool()
{
    while (!pool.empty()) {
        auto c = pool.front();
        pool.pop();
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
