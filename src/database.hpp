#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <jsoncons/json.hpp>
#include <pqxx/pqxx> // Include the libpqxx header for PostgreSQL

using json = jsoncons::json;

class Database {
public:
    Database(std::shared_ptr<pqxx::connection> conn);
    ~Database() = default;

    bool isConnected();
    json executeQuery(const std::string& query);
    json executeReadQuery(const std::string& query);
    bool checkExists(const std::string& table, const std::string& column, const std::string& value);

private:
    std::shared_ptr<pqxx::connection> connection;
};

#endif // DATABASE_HPP
