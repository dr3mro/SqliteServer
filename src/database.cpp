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

        json json_array = json::array();
        json affected_rows;

        affected_rows["affected rows"] = res.affected_rows();
        json_array.push_back(affected_rows);

        for (const auto& row : res) {
            json jsonObj;
            for (const auto& field : row) {
                jsonObj[field.name()] = json::parse(field.as<std::string>());
            }
            json_array.push_back(jsonObj);
        }

        return json_array;
    } catch (const std::exception& e) {
        std::cerr << "Error executing query: " << e.what() << std::endl;
        throw; // Rethrow the exception to indicate failure
    }
}

json Database::executeReadQuery(const std::string& query) // this query has no commit
{
    try {
        pqxx::nontransaction ntxn(*connection);
        pqxx::result res = ntxn.exec(query);

        json json_array = json::array();

        for (const auto& row : res) {
            json jsonObj;
            for (const auto& field : row) {
                jsonObj[field.name()] = json::parse(field.as<std::string>());
            }
            json_array.push_back(jsonObj);
        }

        return json_array;
    } catch (const std::exception& e) {
        std::cerr << "Error executing query: " << e.what() << std::endl;
        throw; // Rethrow the exception to indicate failure
    }
}