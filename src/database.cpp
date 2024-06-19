#include "database.hpp"
#include <iostream>

Database::Database(std::shared_ptr<pqxx::connection> conn)
    : connection(conn.get())
{
    try {
        if (conn->is_open()) {
            std::cout << "Opened database successfully: " << conn->dbname() << std::endl;
        } else {
            std::cout << "Failed to open database" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

bool Database::isConnected()
{
    return connection->is_open();
}

json Database::executeQuery(const std::string& query)
{
    pqxx::work txn(*connection);

    pqxx::result res = txn.exec(query);
    json results = json::array();

    for (const auto& row : res) {
        json jsonRow;
        for (const auto& field : row) {
            jsonRow[field.name()] = field.as<std::string>();
        }
        results.push_back(jsonRow);
    }
    return results;
}

void Database::executeNonQuery(const std::string& query)
{
    pqxx::work txn(*connection);
    txn.exec(query);
    txn.commit();
}
