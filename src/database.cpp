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

void Database::executeQuery(const std::string& query)
{
    try {
        pqxx::work W(*connection);
        W.exec(query);
        W.commit();
        std::cout << "Query executed successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}