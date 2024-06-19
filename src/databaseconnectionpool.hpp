#pragma once
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

#include "database.hpp"

class DatabaseConnectionPool {
public:
    DatabaseConnectionPool(size_t pool_size);
    std::shared_ptr<Database> get_connection();
    void return_connection(std::shared_ptr<Database> db);

private:
    size_t pool_size;
    std::queue<std::shared_ptr<Database>> connections;
    std::vector<std::shared_ptr<Database>> available_connections;
    std::mutex mutex;
    std::condition_variable cv;
};
