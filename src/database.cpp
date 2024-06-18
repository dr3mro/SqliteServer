#include "database.hpp"
#include <iostream>

Database::Database(const std::string& connection_str)
{
    try {
        conn = new pqxx::connection(connection_str);
        if (conn->is_open()) {
            std::cout << "Opened database successfully: " << conn->dbname() << std::endl;
            // SQL command to create a table
            std::string sql = "CREATE TABLE IF NOT EXISTS data ("
                              "id SERIAL PRIMARY KEY, "
                              "value TEXT NOT NULL);";
            this->executeQuery(sql);
        } else {
            std::cout << "Failed to open database" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

Database::~Database()
{
    delete conn;
}

void Database::executeQuery(const std::string& query)
{
    try {
        pqxx::work W(*conn);
        W.exec(query);
        W.commit();
        std::cout << "Query executed successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}