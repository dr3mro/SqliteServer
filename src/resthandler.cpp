#include "resthandler.hpp"
#include <crow.h>
#include <fmt/core.h> // Include fmt library for string formatting
#include <future>
#include <vector>

RestHandler::RestHandler(DatabaseHandler& dbHandler, ThreadPool& threadPool)
    : dbHandler(dbHandler)
    , threadPool(threadPool)
{
}

void RestHandler::create_patient_basic_information(const crow::request& req, crow::response& res)
{
    auto func = [this, &res, &req]() {
        json response_json;

        auto basic_data_json = json::parse(req.body);

        try {
            uint64_t nextid = get_next_patient_id();

            if (nextid == 0) {
                std::cout << "failed to get nextval\n";
                res.code = 401;
                res.write("Failed to create a new patient.");
                return;
            }

            basic_data_json["id"] = nextid;

            // Construct SQL query using {fmt} for parameterized query
            std::string query
                = fmt::format("INSERT INTO patients_basic_data (id, basic_data) VALUES ('{}','{}') RETURNING basic_data;",
                    nextid, basic_data_json.dump());

            // Execute the query using DatabaseHandler
            json query_results_json = dbHandler.executeQuery(query);

            response_json["status message"] = "success";
            response_json["status code"] = 0;
            response_json["response"] = query_results_json;

            // response_json["response"] = "creating patient basic information is successful.";

            res.code = 200;
            res.add_header("Content-Encoding", "gzip");
            res.write(response_json.dump(4));

        } catch (const std::exception& e) {
            // Handle exception (log, etc.)
            response_json["status message"] = "failure";
            response_json["status code"] = -1;
            response_json["response"] = fmt::format("failed: {}", e.what());
            res.code = 500;
            res.write(response_json.dump());
        }
    };

    auto t = threadPool.enqueue(func);
    t.wait(); // Wait for the task to complete
    res.end();
}

void RestHandler::read_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id)
{
    (void)req;
    auto func = [this, &res, id]() {
        json response_json;
        try {
            std::string query
                = fmt::format("SELECT basic_data FROM patients_basic_data WHERE id = {}", id);
            json query_results_json = dbHandler.executeReadQuery(query);

            if (query_results_json.empty()) {
                res.code = 404;
                response_json["status message"] = "failure";
                response_json["status code"] = -1;
                response_json["response"] = query_results_json;
                res.write(response_json.dump());
            } else {
                res.code = 200;
                res.add_header("Content-Encoding", "gzip");
                response_json["status message"] = "success";
                response_json["status code"] = 0;
                response_json["response"] = query_results_json;
                res.write(response_json.dump(4)); // Pretty print JSON with 4 spaces indentation
            }
            res.end();
            return; // Successful query, exit retry loop
        } catch (const std::exception& e) {
            // Handle exception (log, etc.)
            res.code = 500;
            response_json["status message"] = "failure";
            response_json["status code"] = -2;
            response_json["response"] = fmt::format("failed: {}", e.what());
            res.write(response_json.dump(4));
        }
    };

    auto t = threadPool.enqueue(func);
    t.wait(); // Wait for the task to complete
    res.end();
}

void RestHandler::update_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id)
{
    auto func = [this, &res, &req, id]() {
        json response_json;

        auto basic_data_json = json::parse(req.body);

        try {
            // Construct SQL query using {fmt} for parameterized query
            std::string query
                = fmt::format("UPDATE patients_basic_data SET basic_data = '{}' WHERE id = '{}' RETURNING basic_data;",
                    basic_data_json.dump(), id);

            // Execute the query using DatabaseHandler
            json query_results_json = dbHandler.executeQuery(query);

            response_json["status message"] = "success";
            response_json["status code"] = 0;
            response_json["response"] = query_results_json;

            res.code = 200;
            res.add_header("Content-Encoding", "gzip");
            res.write(response_json.dump(4));

        } catch (const std::exception& e) {
            // Handle exception (log, etc.)
            response_json["status message"] = "failure";
            response_json["status code"] = -1;
            response_json["response"] = fmt::format("failed: {}", e.what());
            res.code = 500;
            res.write(response_json.dump());
        }
    };

    auto t = threadPool.enqueue(func);
    t.wait(); // Wait for the task to complete
    res.end();
}
void RestHandler::delete_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id)
{
    (void)req;

    auto func = [this, &res, id]() {
        json response_json;

        try {
            // Construct SQL query using {fmt} for parameterized query
            std::string query = fmt::format("DELETE * FROM patients_basic_data WHERE id={};", id);

            // Execute the query using DatabaseHandler
            json results = dbHandler.executeQuery(query);

            res.code = 200;
            res.add_header("Content-Encoding", "gzip");
            res.write(results.dump(4));

        } catch (const std::exception& e) {
            // Handle exception (log, etc.)
            res.code = 500;
            response_json["status id"] = -2;
            response_json["status message"] = fmt::format("failed: {}", e.what());
            response_json["response"] = fmt::format("failed: {}", e.what());
            res.write(response_json.dump());
        }
    };

    auto t = threadPool.enqueue(func);
    t.wait(); // Wait for the task to complete
    res.end();
}

uint64_t RestHandler::get_next_patient_id()
{
    json json_nextval = dbHandler.executeQuery("SELECT NEXTVAL('patient_id_seq');");

    // Iterate through each object in the JSON array
    for (const auto& obj : json_nextval) {
        if (obj.contains("nextval")) {
            return obj["nextval"];
        }
    }
    return 0;
}