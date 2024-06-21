#include "resthandler.hpp"
#include <crow.h>
#include <fmt/core.h> // Include fmt library for string formatting
#include <future>

using json = nlohmann::json;

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
    auto func = [this, &res, &req]() {
        json response_json = {
            { "id", "0" },
            { "msg", "msg" },
        };

        auto jsonData = json::parse(req.body);

        uint64_t id = jsonData["id"].get<uint64_t>();
        std::string_view name = jsonData["name"].get<std::string_view>();
        std::string_view phone = jsonData["phone"].get<std::string_view>();

        // Validate input (optional)
        if (id != 0 || name.empty() || phone.empty()) {
            res.code = 400;

            response_json["id"] = -1;
            response_json["msg"] = "Bad Request: id, name or phone must be provided.";

            res.write(response_json.dump());
            return;
        }

        try {
            // Construct SQL query using {fmt} for parameterized query
            std::string query = fmt::format("INSERT INTO personal_history (name ,phone,json) VALUES ('{}','{}','{}') RETURNING id;",
                name, phone, jsonData.dump());
            std::cout << "Query: " << query << '\n';
            // Execute the query using DatabaseHandler
            std::string new_id = dbHandler.executeQuery(query);

            res.code = 200;
            response_json["id"] = std::stoi(new_id);
            response_json["msg"] = "Inserted successfully.";
            res.write("00000");
            // res.write(response_json.dump());
        } catch (const std::exception& e) {
            // Handle exception (log, etc.)
            res.code = 500;

            response_json["id"] = -2;
            response_json["msg"] = fmt::format("failed: {}", e.what());

            res.write(response_json.dump());
        }
    };

    auto t = threadPool.enqueue(func);
    t.wait(); // Wait for the task to complete
    res.end();
}
