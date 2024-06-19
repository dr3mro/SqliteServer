#include "resthandler.hpp"

RestHandler::RestHandler(ConnectionPool& pool)
    : connection_pool(pool)
{
}

void RestHandler::handle_get(const crow::request& req, crow::response& res)
{
    // Retrieve a connection from the pool
    auto db = connection_pool.get_connection();

    try {
        // Execute SQL query
        db->executeQuery("SELECT * FROM your_table");

        // Process the result (dummy example)
        // for (auto row : result) {
        //     std::string id = row["id"].as<std::string>();
        //     std::string value = row["value"].as<std::string>();
        //     res.write(id + ": " + value + "\n");
        // }

    } catch (const std::exception& e) {
        res.code = 500;
        res.write("Error: " + std::string(e.what()));
    }

    // Return the connection to the pool
    connection_pool.return_connection(db);
    res.end();
}

void RestHandler::handle_post(const crow::request& req, crow::response& res)
{
    // Retrieve a connection from the pool
    auto db = connection_pool.get_connection();

    try {
        // Extract data from request
        auto json_data = crow::json::load(req.body);
        if (!json_data) {
            throw std::invalid_argument("Invalid JSON");
        }
        std::string id = json_data["id"].s();
        std::string value = json_data["value"].s();

        // db->executeQuery("INSERT INTO your_table (id, value) VALUES ($1, $2)", id, value);

        res.code = 201; // Created
        res.write("Inserted data with id: " + id);
    } catch (const std::exception& e) {
        res.code = 500;
        res.write("Error: " + std::string(e.what()));
    }

    // Return the connection to the pool
    connection_pool.return_connection(db);
    res.end();
}