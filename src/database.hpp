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

    template <typename T>
    T doSimpleQuery(const std::string& query)
    {
        try {
            pqxx::nontransaction txn(*connection);
            pqxx::result result = txn.exec(query);
            return result[0][0].as<T>();
        } catch (const std::exception& e) {
            std::cerr << "Error executing query: " << e.what() << std::endl;
            throw; // Rethrow the exception to indicate failure
        }
    }

private:
    std::shared_ptr<pqxx::connection> connection;
};

#endif // DATABASE_HPP
