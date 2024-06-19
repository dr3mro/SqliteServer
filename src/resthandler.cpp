#include "resthandler.hpp"
#include <crow.h>
#include <fmt/core.h> // Include fmt library for string formatting
#include <iomanip> // for std::quoted

RestHandler::RestHandler(DatabaseHandler& dbHandler, ThreadPool& threadPool)
    : dbHandler(dbHandler)
    , threadPool(threadPool)
{
}

void RestHandler::handle_get(const crow::request& req, crow::response& res, int id)
{
    (void)req;
    threadPool.enqueue([this, &res, id]() {
        try {
            std::string query = fmt::format("SELECT * FROM data WHERE id = {}", id);
            json result = dbHandler.executeQuery(query);

            if (result.empty()) {
                res.code = 404;
                res.write("Not Found");
            } else {
                res.code = 200;
                res.write(result.dump(4)); // Pretty print JSON with 4 spaces indentation
            }
        } catch (const std::exception& e) {
            res.code = 500;
            res.write("Error: " + std::string(e.what()));
        }

        res.end();
    });
}

void RestHandler::handle_post(const crow::request& req, crow::response& res)
{
    threadPool.enqueue([this, &res, &req]() {
        try {
            auto jsonData = json::parse(req.body);
            int id = jsonData["id"];
            std::string value = jsonData["value"];

            // Validate input (optional)
            if (id <= 0 || value.empty()) {
                res.code = 400;
                res.write("Bad Request: Invalid id or value");
                res.end();
                return;
            }

            // Construct SQL query using {fmt} for parameterized query
            std::string query = fmt::format("INSERT INTO data (id, value) VALUES ({}, {})",
                id, value);

            // Execute the query using DatabaseHandler
            dbHandler.executeNonQuery(query);

            res.code = 200;
            res.write("Inserted successfully");
        } catch (const std::exception& e) {
            res.code = 500;
            res.write("Error: " + std::string(e.what()));
        }

        res.end();
    });
}
