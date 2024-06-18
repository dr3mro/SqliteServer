#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <pqxx/pqxx> // Include the libpqxx header for PostgreSQL

class Database {
public:
    Database(const std::string& connection_str);
    ~Database();

    void executeQuery(const std::string& query);

private:
    pqxx::connection* conn;
};

#endif // DATABASE_HPP
