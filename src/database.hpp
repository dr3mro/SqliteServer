#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <pqxx/pqxx> // Include the libpqxx header for PostgreSQL

class Database {
public:
    Database(std::shared_ptr<pqxx::connection> conn);
    ~Database() = default;

    bool isConnected();
    void executeQuery(const std::string& query);

private:
    std::shared_ptr<pqxx::connection> connection;
};

#endif // DATABASE_HPP
