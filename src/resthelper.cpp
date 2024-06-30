#include "resthelper.hpp"
#include "fmt/core.h"
#include <jwt-cpp/jwt.h>
#include <picosha2.h>

RestHelper::RestHelper(DatabaseController& dbController)
    : dbController(dbController)
{
}

uint64_t RestHelper::get_next_patient_id()
{
    json json_nextval = dbController.executeQuery("SELECT NEXTVAL('patient_id_seq');");

    // Iterate through each object in the JSON array
    for (const auto& obj : json_nextval.array_range()) {
        if (obj.contains("nextval")) {
            return obj["nextval"].as<uint64_t>();
        }
    }
    return 0;
}

bool RestHelper::check_affected_rows(const json& response)
{
    for (const auto& obj : response.array_range()) {
        if (obj.contains("affected rows")) {
            return obj["affected rows"].as<uint64_t>() == 1;
        }
    }
    return false;
}

void RestHelper::evaluate_response(json& response_json, const json& query_results_json)
{
    if (check_affected_rows(query_results_json))
        format_response(response_json, 0, "success", query_results_json);
    else
        format_response(response_json, -1, "failure", query_results_json);
}
void RestHelper::format_response(json& response_json, const short status, const std::string& status_message, const json& response)
{
    response_json["status id"] = status;
    response_json["status message"] = status_message;
    response_json["response"] = response;
}

void RestHelper::finish_response(crow::response& res, const int& code, const json& response_json)
{
    std::string body;
    response_json.dump(body, jsoncons::indenting::indent);
    res.code = code;
    res.write(body);
    res.end();
}

bool RestHelper::is_request_data_valid(const crow::request& req, crow::response& res, json& response_json, json& data_json)
{
    // Try to parse json and throw error if invalid json and if parsed check integreity by claculating sha256
    try {
        data_json = json::parse(req.body);
        // std::cout << data_json.to_string() << std::endl;

        if (!data_json.contains("sha256sum")) {
            return false;
        }
        if (!data_json.contains("payload")) {
            return false;
        }

        std::string sha256sum = data_json["sha256sum"].as<std::string>();
        std::string payload = data_json["payload"].as<std::string>();
        // std::cout << picosha2::hash256_hex_string(payload) << std::endl;
        if (sha256sum == picosha2::hash256_hex_string(payload)) {
            return true;
        }
    } catch (const std::exception& e) {
        respond_with_error(res, response_json, "Failure, invalid JSON", fmt::format("Error parsing data: {}", e.what()), -1, 400);
    }
    return false;
}
bool RestHelper::is_login_data_valid(const crow::request& req, crow::response& res, json& response_json, json& data_json)
{
    // Try to parse json and throw error if invalid json and if parsed check integreity by claculating sha256
    try {
        data_json = json::parse(req.body);
        return true;
    } catch (const std::exception& e) {
        respond_with_error(res, response_json, "Failed to create a new user, invalid JSON", fmt::format("Error parsing user data: {}", e.what()), -1, 400);
        return false;
    }
}

void RestHelper::respond_with_error(crow::response& res, json& response_json, const std::string& status_message, const std::string& response, const short status, const short code)
{
    format_response(response_json, status, status_message, response);
    finish_response(res, code, response_json);
}