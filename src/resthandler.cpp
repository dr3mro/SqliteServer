#include "resthandler.hpp"
#include <crow.h>
#include <fmt/core.h> // Include fmt library for string formatting
#include <future>

RestHandler::RestHandler(DatabaseHandler& dbHandler, ThreadPool& threadPool)
    : dbHandler(dbHandler)
    , threadPool(threadPool)
{
}

void RestHandler::handle_get(const crow::request& req, crow::response& res, int id)
{
    (void)req;
    int max_retry_attempts = 5; // Maximum number of retry attempts
    int retry_delay_seconds = 1; // Initial retry delay in seconds

    auto retry_func = [this, &res, id, &max_retry_attempts, &retry_delay_seconds]() {
        for (int attempt = 1; attempt <= max_retry_attempts; ++attempt) {
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
                res.end();
                return; // Successful query, exit retry loop
            } catch (const std::exception& e) {
                // Handle exception (log, etc.)
                res.code = 500;
                res.write(fmt::format("Attempt {} failed: {}", attempt, e.what()));

                if (attempt < max_retry_attempts) {
                    // Sleep before retrying
                    std::this_thread::sleep_for(std::chrono::seconds(retry_delay_seconds));
                    // Increase delay for next retry (exponential backoff)
                    retry_delay_seconds *= 2;
                }
            }
        }
        // If all retries fail, end the response
        res.end();
    };

    auto t = threadPool.enqueue(retry_func);
    t.wait(); // Wait for the task to complete
}

void RestHandler::handle_post(const crow::request& req, crow::response& res)
{
    int max_retry_attempts = 5; // Maximum number of retry attempts
    int retry_delay_seconds = 1; // Initial retry delay in seconds

    auto retry_func = [this, &res, &req, &max_retry_attempts, &retry_delay_seconds]() {
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

        for (int attempt = 1; attempt <= max_retry_attempts; ++attempt) {
            try {
                // Construct SQL query using {fmt} for parameterized query
                std::string query = fmt::format("INSERT INTO data (id, value) VALUES ({}, '{}')",
                    id, value);

                // Execute the query using DatabaseHandler
                dbHandler.executeNonQuery(query);

                res.code = 200;
                res.write("Inserted successfully");
                res.end();
                return; // Successful insert, exit retry loop
            } catch (const std::exception& e) {
                // Handle exception (log, etc.)
                res.code = 500;
                res.write(fmt::format("Attempt {} failed: {}", attempt, e.what()));

                if (attempt < max_retry_attempts) {
                    // Sleep before retrying
                    std::this_thread::sleep_for(std::chrono::seconds(retry_delay_seconds));
                    // Increase delay for next retry (exponential backoff)
                    retry_delay_seconds *= 2;
                }
            }
        }
        // If all retries fail, end the response
        res.end();
    };

    auto t = threadPool.enqueue(retry_func);
    t.wait(); // Wait for the task to complete
}
