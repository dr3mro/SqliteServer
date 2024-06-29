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

void PatientController::create_new_patient(const crow::request& req, crow::response& res)
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
            rHelper.respond_with_error(res, response_json, "failed to create a new patient", "authentication token invalidated", -1, 400);
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

        // Extract keys from basic_data
        std::vector<std::string> keys;
        std::vector<std::string> values;
        std::stringstream sql;

        sql << "INSERT INTO patients (id";

        for (auto it = data_json["payload"].object_range().begin(); it != data_json["payload"].object_range().end(); ++it) {
            keys.push_back(it->key());
            values.push_back(it->value().to_string());
        }

        // Join keys with commas
        bool first = true;
        for (const auto& key : keys) {
            sql << ", ";
            sql << key;
        }

        // Construct values for VALUES clause
        sql << " ) VALUES ( " << nextid;

        for (const auto& value : values) {
            sql << ", ";
            sql << "'" << value << "'";
        }

        sql << " ) RETURNING id;";

        json query_results_json = dbController.executeQuery(sql.str());

        rHelper.evaluate_response(response_json, query_results_json);
        rHelper.finish_response(res, 200, response_json);
    } catch (const std::exception& e) {
        // Handle exception (log, etc.)
        rHelper.format_response(response_json, -2, "failure", fmt::format("failed: {}", e.what()));
        rHelper.finish_response(res, 500, response_json);
    }
}

void PatientController::get_patient_data(const crow::request& req, crow::response& res)
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

void PatientController::update_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id)
{
    json response_json;

    auto basic_data_json = json::parse(req.body);

    try {
        // Construct SQL query using {fmt} for parameterized query
        std::string query
            = fmt::format("UPDATE patients_basic_data SET basic_data = '{}' WHERE id = '{}' RETURNING basic_data;",
                basic_data_json.to_string(), id);

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
void PatientController::delete_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id)
{

    json response_json;

    try {
        // Construct SQL query using {fmt} for parameterized query
        std::string query = fmt::format("DELETE FROM patients_basic_data WHERE id={};", id);

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
