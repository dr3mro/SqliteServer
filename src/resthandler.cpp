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

void RestHandler::handle_get_patient_basic_information(const crow::request& req, crow::response& res, int id)
{
    (void)req;
    auto retry_func = [this, &res, id]() {
        try {
            std::string query = fmt::format("SELECT json FROM personal_history WHERE id = {}", id);
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
            res.write(fmt::format("failed: {}", e.what()));
        }
    };

    auto t = threadPool.enqueue(retry_func);
    t.wait(); // Wait for the task to complete
    res.end();
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
            response_json["id"] = -1;
            response_json["msg"] = "Bad Request: id, name or phone must be provided.";
            res.code = 400;
            res.write(response_json.dump());
            return;
        }

        try {
            // Construct SQL query using {fmt} for parameterized query
            std::string query = fmt::format("INSERT INTO personal_history (name ,phone,json) VALUES ('{}','{}','{}') RETURNING id;",
                name, phone, jsonData.dump());

            // Execute the query using DatabaseHandler
            json results = dbHandler.executeQuery(query);

            response_json["id"] = results[0]["id"];
            response_json["msg"] = "Inserted successfully.";
            res.code = 200;
            res.write(response_json.dump());

        } catch (const std::exception& e) {
            // Handle exception (log, etc.)
            response_json["id"] = -2;
            response_json["msg"] = fmt::format("failed: {}", e.what());
            res.code = 500;
            res.write(response_json.dump());
        }
    };

    auto t = threadPool.enqueue(func);
    t.wait(); // Wait for the task to complete
    res.end();
}
