#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <memory>
#include <mutex>
#include <queue>

class ConnectionPool {
public:
    ConnectionPool(const std::string& dbFile, size_t poolSize);
    void returnConnection(std::shared_ptr<SQLite::Database> conn);
    ~ConnectionPool();

    std::shared_ptr<SQLite::Database> getConnection();

private:
    std::queue<std::shared_ptr<SQLite::Database>> pool;
    std::mutex pool_mutex;
};

#endif // CONNECTION_POOL_H
