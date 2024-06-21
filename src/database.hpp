#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <nlohmann/json.hpp>
#include <pqxx/pqxx> // Include the libpqxx header for PostgreSQL

using json = nlohmann::json;

class Database {
public:
    Database(std::shared_ptr<pqxx::connection> conn);
    ~Database() = default;

    bool isConnected();
    json executeQuery(const std::string& query);

private:
    std::shared_ptr<pqxx::connection> connection;
};

#endif // DATABASE_HPP
