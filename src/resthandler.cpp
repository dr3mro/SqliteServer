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
                format_response(response_json, -1, "failed to create a new patient", "failed to get nextval");
                return finish_response(res, 401, response_json.dump(4));
            }

            basic_data_json["id"] = nextid;

            // Construct SQL query using {fmt} for parameterized query
            std::string query
                = fmt::format("INSERT INTO patients_basic_data (id, basic_data) VALUES ('{}','{}') RETURNING basic_data;",
                    nextid, basic_data_json.dump());

            // Execute the query using DatabaseHandler
            json query_results_json = dbHandler.executeQuery(query);

            make_response(response_json, query_results_json);
            return finish_response(res, 200, response_json.dump(4));
        } catch (const std::exception& e) {
            // Handle exception (log, etc.)
            format_response(response_json, -2, "failure", fmt::format("failed: {}", e.what()));
            return finish_response(res, 500, response_json.dump(4));
        }
    };

    auto t = threadPool.enqueue(func);
    t.wait(); // Wait for the task to complete
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
                format_response(response_json, -1, "not found", query_results_json);
                return finish_response(res, 404, response_json.dump(4));
            } else {
                make_response(response_json, query_results_json);
                return finish_response(res, 200, response_json.dump(4));
            }
            return; // Successful query, exit retry loop
        } catch (const std::exception& e) {
            // Handle exception (log, etc.)
            format_response(response_json, -2, "failure", fmt::format("failed: {}", e.what()));
            return finish_response(res, 500, response_json.dump(4));
        }
    };

    auto t = threadPool.enqueue(func);
    t.wait(); // Wait for the task to complete
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

            make_response(response_json, query_results_json);
            return finish_response(res, 200, response_json.dump(4));

        } catch (const std::exception& e) {
            // Handle exception (log, etc.)
            format_response(response_json, -2, "failure", fmt::format("failed: {}", e.what()));
            return finish_response(res, 500, response_json.dump(4));
        }
    };

    auto t = threadPool.enqueue(func);
    t.wait(); // Wait for the task to complete
}
void RestHandler::delete_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id)
{
    (void)req;

    auto func = [this, &res, id]() {
        json response_json;

        try {
            // Construct SQL query using {fmt} for parameterized query
            std::string query = fmt::format("DELETE FROM patients_basic_data WHERE id={};", id);

            // Execute the query using DatabaseHandler
            json query_results_json = dbHandler.executeQuery(query);

            make_response(response_json, query_results_json);
            return finish_response(res, 200, response_json.dump(4));

        } catch (const std::exception& e) {
            // Handle exception (log, etc.)
            format_response(response_json, -2, "failure", fmt::format("failed: {}", e.what()));
            return finish_response(res, 500, response_json.dump(4));
        }
    };

    auto t = threadPool.enqueue(func);
    t.wait(); // Wait for the task to complete
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
bool RestHandler::check_affected_rows(const json& response)
{

    for (const auto& obj : response) {
        if (obj.contains("affected rows")) {
            return obj["affected rows"] == 1;
        }
    }
    return false;
}

void RestHandler::make_response(json& response_json, const json& query_results_json)
{
    if (check_affected_rows(query_results_json))
        format_response(response_json, 0, "success", query_results_json);
    else
        format_response(response_json, -1, "failure", query_results_json);
}
void RestHandler::format_response(json& response_json, const short status, const std::string& status_message, const json& response)
{
    response_json["status id"] = status;
    response_json["status message"] = status_message;
    response_json["response"] = response;
}

void RestHandler::finish_response(crow::response& res, const int& code, const std::string& body)
{
    res.code = code;
    res.write(body);
    res.end();
}