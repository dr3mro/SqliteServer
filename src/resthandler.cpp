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

void RestHandler::handle_create_client_personal_history(const crow::request& req, crow::response& res)
{
    nlohmann::json response_json_object = {
        { "id", "0" },
        { "msg", "msg" },
    };

    auto func = [this, &res, &req, &response_json_object]() {
        auto jsonData = json::parse(req.body);

        uint64_t id = jsonData["id"];
        std::string name = jsonData["name"];
        std::string phone = jsonData["phone"];

        // Validate input (optional)
        if (id != 0 || name.empty() || phone.empty()) {
            res.code = 400;

            response_json_object["id"] = -1;
            response_json_object["msg"] = "Bad Request: id, name or phone must be provided.";

            res.write(response_json_object.dump());
            res.end();
            return;
        }

        try {
            // Construct SQL query using {fmt} for parameterized query
            std::string query = fmt::format("INSERT INTO personal_history (name ,phone,json) VALUES ('{}','{}','{}') RETURNING id;",
                name, phone, jsonData.dump());

            // Execute the query using DatabaseHandler
            std::string new_id = dbHandler.executeQuery(query);

            res.code = 200;
            response_json_object["id"] = std::stoi(new_id);
            response_json_object["msg"] = "Inserted successfully.";

            res.write(response_json_object.dump());
            res.end();
            return;
        } catch (const std::exception& e) {
            // Handle exception (log, etc.)
            res.code = 500;

            response_json_object["id"] = -2;
            response_json_object["msg"] = fmt::format("failed: {}", e.what());

            res.write(response_json_object.dump());
        }
        res.end();
    };

    auto t = threadPool.enqueue(func);
    t.wait(); // Wait for the task to complete
}
