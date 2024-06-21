#include "database.hpp"
#include <iostream>

Database::Database(std::shared_ptr<pqxx::connection> conn)
    : connection(conn)
{
    if (connection->is_open()) {
        std::cout << "Opened database successfully: " << conn->dbname() << std::endl;
    } else {
        std::cerr << "Failed to open database" << std::endl;
    }
}

bool Database::isConnected()
{
    return connection->is_open();
}

json Database::executeQuery(const std::string& query)
{
    try {
        pqxx::work txn(*connection);
        pqxx::result res = txn.exec(query);
        txn.commit();
        json results = json::array();

        for (const auto& row : res) {
            json jsonRow;
            for (const auto& field : row) {
                jsonRow[field.name()] = field.as<std::string>();
            }
            results.push_back(jsonRow);
        }

        return results;
    } catch (const std::exception& e) {
        std::cerr << "Error executing query: " << e.what() << std::endl;
        throw; // Rethrow the exception to indicate failure
    }
}
