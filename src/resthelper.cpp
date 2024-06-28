#include "resthelper.hpp"
#include "fmt/core.h"
#include <jwt-cpp/jwt.h>

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

bool RestHelper::is_request_data_valid(const crow::request& req, crow::response& res, json& response_json)
{
    // Try to parse json and throw error if invalid json
    try {
        auto userdata_json = json::parse(req.body);
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