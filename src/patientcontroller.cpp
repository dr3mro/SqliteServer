#include "patientcontroller.hpp"
#include <crow.h>
#include <fmt/core.h> // Include fmt library for string formatting
#include <fmt/ranges.h>
#include <vector>

PatientController::PatientController(DatabaseController& dbController, RestHelper& rHelper, Tokenizer& tokenizer)
    : dbController(dbController)
    , rHelper(rHelper)
    , tokenizer(tokenizer)
{
}

void PatientController::create_patient(const crow::request& req, crow::response& res)
{
    json response_json;

    try {
        // Data Integrity check
        json data_json;
        if (!rHelper.is_request_data_valid(req, res, response_json, data_json)) {
            rHelper.respond_with_error(res, response_json, "failed to create a new patient", "payload integrity check failed", -1, 400);
            return;
        }

        // User authentication check
        std::string username = data_json["username"].as<std::string>();
        std::string token = data_json["token"].as<std::string>();

        if (!tokenizer.token_validator(token, username)) {
            rHelper.respond_with_error(res, response_json, "failed to create a new patient", "authentication token invalid or expired", -1, 400);
            return;
        }

        // Find the next ID
        uint64_t nextid
            = rHelper.get_next_patient_id();

        if (nextid == 0) {
            rHelper.format_response(response_json, -1, "failed to create a new patient", "failed to get nextval");
            rHelper.finish_response(res, 401, response_json);
            return;
        }

        std::vector<std::string> keys_arr;
        std::vector<std::string> values_arr;

        for (auto it = data_json["payload"].object_range().begin(); it != data_json["payload"].object_range().end(); ++it) {

            if (it->key() == "basic_data" && it->value().contains("id")) {
                it->value()["id"] = nextid;
            }

            keys_arr.push_back(it->key());
            values_arr.push_back(it->value().as<std::string>());
        }

        std::string columns = fmt::format("{}", fmt::join(keys_arr, ","));
        std::string values = fmt::format("'{}'", fmt::join(values_arr, "','"));

        std::string query = fmt::format("INSERT INTO patients (id, {}) VALUES ({},{}) RETURNING id,{};", columns, nextid, values, columns);

        json query_results_json = dbController.executeQuery(query);

        rHelper.evaluate_response(response_json, query_results_json);
        rHelper.finish_response(res, 200, response_json);
    } catch (const std::exception& e) {
        // Handle exception (log, etc.)
        rHelper.format_response(response_json, -2, "failure", fmt::format("failed: {}", e.what()));
        rHelper.finish_response(res, 500, response_json);
    }
}

void PatientController::read_patient(const crow::request& req, crow::response& res)
{
    json response_json;
    try {
        typedef struct {
            uint64_t user_id;
            std::vector<std::string> schema;
            std::string username;
            std::string token;

            void fill(const json& data)
            {
                user_id = data["id"].as<uint64_t>();
                schema = data["schema"].as<std::vector<std::string>>();
                username = data["username"].as<std::string>();
                token = data["token"].as<std::string>();
            }
        } PatientData;

        PatientData patientData;
        json data_json = json::parse(req.body);
        patientData.fill(data_json);

        if (!tokenizer.token_validator(patientData.token, patientData.username)) {
            rHelper.respond_with_error(res, response_json, "failed to retrieve patient", "token is invalidated", -1, 400);
            return;
        }

        std::string columns = fmt::format("{}", fmt::join(patientData.schema, ", "));

        std::string query
            = fmt::format("SELECT {} FROM patients WHERE id={}", columns, patientData.user_id);
        json query_results_json = dbController.executeReadQuery(query);

        if (query_results_json.empty()) {
            rHelper.format_response(response_json, -1, "not found", query_results_json);
            rHelper.finish_response(res, 404, response_json);
        } else {
            rHelper.format_response(response_json, 0, "success", query_results_json);
            rHelper.finish_response(res, 200, response_json);
        }
    } catch (const std::exception& e) {
        // Handle exception (log, etc.)
        rHelper.format_response(response_json, -2, "failure", fmt::format("failed: {}", e.what()));
        rHelper.finish_response(res, 500, response_json);
    }
}

void PatientController::update_patient(const crow::request& req, crow::response& res)
{
    json response_json;

    try {
        // Data Integrity check
        json data_json;
        if (!rHelper.is_request_data_valid(req, res, response_json, data_json)) {
            rHelper.respond_with_error(res, response_json, "failed to update patient", "payload integrity check failed", -1, 400);
            return;
        }

        // User authentication check
        std::string username = data_json["username"].as<std::string>();
        std::string token = data_json["token"].as<std::string>();

        if (!tokenizer.token_validator(token, username)) {
            rHelper.respond_with_error(res, response_json, "failed to update patient", "authentication token invalid or expired", -1, 400);
            return;
        }

        json payload = data_json["payload"];
        json basic_data = payload["basic_data"];
        uint64_t id = basic_data["id"].as<uint64_t>();

        std::string update_column_values;

        for (auto it = payload.object_range().begin(); it != payload.object_range().end(); ++it) {
            update_column_values.append(fmt::format(" {} = '{}' ", it->key(), it->value().as<std::string>()));
            if (std::next(it) != payload.object_range().end()) {
                update_column_values.append(",");
            }
        }

        std::string query = fmt::format("UPDATE patients set {} WHERE id={};", update_column_values, id);

        std::cout << query << std::endl;

        json query_results_json = dbController.executeQuery(query);

        rHelper.evaluate_response(response_json, query_results_json);
        rHelper.finish_response(res, 200, response_json);
    } catch (const std::exception& e) {
        // Handle exception (log, etc.)
        rHelper.format_response(response_json, -2, "failure", fmt::format("failed: {}", e.what()));
        rHelper.finish_response(res, 500, response_json);
    }
}
void PatientController::delete_patient(const crow::request& req, crow::response& res)
{

    json response_json;

    try {
        // Data Integrity check
        json data_json;
        if (!rHelper.is_request_data_valid(req, res, response_json, data_json)) {
            rHelper.respond_with_error(res, response_json, "failed to delete patient", "payload integrity check failed", -1, 400);
            return;
        }

        // User authentication check
        std::string username = data_json["username"].as<std::string>();
        std::string token = data_json["token"].as<std::string>();

        if (!tokenizer.token_validator(token, username)) {
            rHelper.respond_with_error(res, response_json, "failed to delete patient", "authentication token invalid or expired", -1, 400);
            return;
        }

        json basic_data_json = data_json["payload"]["basic_data"];
        uint64_t user_id = basic_data_json["id"].as<uint64_t>();
        std::string basic_data = basic_data_json.to_string();

        // Construct SQL query using {fmt} for parameterized query
        std::string query
            = fmt::format("DELETE FROM patients where id={} and basic_data @> '{}';", user_id, basic_data);

        std::cout << query << std::endl;

        // Execute the query using DatabaseHandler
        json query_results_json = dbController.executeQuery(query);

        rHelper.evaluate_response(response_json, query_results_json);
        rHelper.finish_response(res, 200, response_json);

    } catch (const std::exception& e) {
        // Handle exception (log, etc.)
        rHelper.format_response(response_json, -2, "failure", fmt::format("failed: {}", e.what()));
        rHelper.finish_response(res, 500, response_json);
    }
}

void PatientController::search_patient(const crow::request& req, crow::response& res)
{
    json response_json;
    try {

        json req_json = json::parse(req.body);
        if (!req_json.contains("keyword")) {
            rHelper.respond_with_error(res, response_json, "failed to execute search", "keyword is not found", -1, 400);
            return;
        }

        if (!req_json.contains("order_by")) {
            rHelper.respond_with_error(res, response_json, "failed to execute search", "order_by is not found", -1, 400);
            return;
        }
        if (!req_json.contains("direction")) {
            rHelper.respond_with_error(res, response_json, "failed to execute search", "direction is not found", -1, 400);
            return;
        }

        if (!req_json.contains("limit")) {
            rHelper.respond_with_error(res, response_json, "failed to execute search", "limit is not found", -1, 400);
            return;
        }

        if (!req_json.contains("offset")) {
            rHelper.respond_with_error(res, response_json, "failed to execute search", "offset is not found", -1, 400);
            return;
        }

        if (!req_json.contains("username")) {
            rHelper.respond_with_error(res, response_json, "failed to execute search", "username is not found", -1, 400);
            return;
        }
        if (!req_json.contains("token")) {
            rHelper.respond_with_error(res, response_json, "failed to execute search", "token is not found", -1, 400);
            return;
        }

        std::string keyword = req_json["keyword"].as<std::string>();
        std::string order_by = req_json["order_by"].as<std::string>();
        std::string direction = req_json["direction"].as<short>() == 0 ? "ASC" : "DESC";
        size_t limit = req_json["limit"].as<size_t>();
        size_t offset = req_json["offset"].as<size_t>();
        std::string username = req_json["username"].as<std::string>();
        std::string token = req_json["token"].as<std::string>();

        if (!tokenizer.token_validator(token, username)) {
            rHelper.respond_with_error(res, response_json, "failed to execute search", "token is invalidated", -1, 400);
            return;
        }

        std::string query
            = fmt::format("SELECT basic_data FROM patients WHERE basic_data::text ILIKE '%{}%' ORDER BY {} {} LIMIT {} OFFSET {};", keyword, order_by, direction, limit + 1, offset);

        json query_results_json = dbController.executeReadQuery(query);

        size_t results_count = query_results_json.size();

        if (results_count > limit) {
            response_json["more"] = true;
            response_json["offset"] = offset + limit;
            query_results_json.erase(query_results_json.array_range().end() - 1);

        } else {
            response_json["more"] = false;
            response_json["offset"] = 0;
        }

        if (query_results_json.empty()) {
            rHelper.format_response(response_json, -1, "not found", query_results_json);
            rHelper.finish_response(res, 404, response_json);
        } else {
            rHelper.format_response(response_json, 0, "success", query_results_json);
            rHelper.finish_response(res, 200, response_json);
        }
    } catch (const std::exception& e) {
        // Handle exception (log, etc.)
        rHelper.format_response(response_json, -2, "failure", fmt::format("failed: {}", e.what()));
        rHelper.finish_response(res, 500, response_json);
    }
}