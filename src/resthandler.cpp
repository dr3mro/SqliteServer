#include "resthandler.hpp"
#include <crow.h>
#include <fmt/core.h> // Include fmt library for string formatting
#include <picosha2.h>
#include <regex>

RestHandler::RestHandler(DatabaseHandler& dbHandler)
    : dbHandler(dbHandler)
{
    options.indent_size(4);
}

void RestHandler::create_patient_basic_information(const crow::request& req, crow::response& res)
{
    json response_json;
    auto basic_data_json = json::parse(req.body);

    try {
        uint64_t nextid = get_next_patient_id();

        if (nextid == 0) {
            format_response(response_json, -1, "failed to create a new patient", "failed to get nextval");
            finish_response(res, 401, response_json);
        }

        basic_data_json["id"] = nextid;
        // Construct SQL query using {fmt} for parameterized query
        std::string query
            = fmt::format("INSERT INTO patients_basic_data (id, basic_data) VALUES ('{}','{}') RETURNING basic_data;",
                nextid, basic_data_json.to_string());

        // Execute the query using DatabaseHandler
        json query_results_json = dbHandler.executeQuery(query);
        evaluate_response(response_json, query_results_json);
        finish_response(res, 200, response_json);
    } catch (const std::exception& e) {
        // Handle exception (log, etc.)
        format_response(response_json, -2, "failure", fmt::format("failed: {}", e.what()));
        finish_response(res, 500, response_json);
    }
}

void RestHandler::read_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id)
{
    json response_json;
    try {
        std::string query
            = fmt::format("SELECT basic_data FROM patients_basic_data WHERE id = {}", id);
        json query_results_json = dbHandler.executeReadQuery(query);

        if (query_results_json.empty()) {
            format_response(response_json, -1, "not found", query_results_json);
            finish_response(res, 404, response_json);
        } else {
            format_response(response_json, 0, "success", query_results_json);
            finish_response(res, 200, response_json);
        }
    } catch (const std::exception& e) {
        // Handle exception (log, etc.)
        format_response(response_json, -2, "failure", fmt::format("failed: {}", e.what()));
        finish_response(res, 500, response_json);
    }
}

void RestHandler::update_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id)
{
    json response_json;

    auto basic_data_json = json::parse(req.body);

    try {
        // Construct SQL query using {fmt} for parameterized query
        std::string query
            = fmt::format("UPDATE patients_basic_data SET basic_data = '{}' WHERE id = '{}' RETURNING basic_data;",
                basic_data_json.to_string(), id);

        // Execute the query using DatabaseHandler
        json query_results_json = dbHandler.executeQuery(query);

        evaluate_response(response_json, query_results_json);
        finish_response(res, 200, response_json);

    } catch (const std::exception& e) {
        // Handle exception (log, etc.)
        format_response(response_json, -2, "failure", fmt::format("failed: {}", e.what()));
        finish_response(res, 500, response_json);
    }
}
void RestHandler::delete_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id)
{

    json response_json;

    try {
        // Construct SQL query using {fmt} for parameterized query
        std::string query = fmt::format("DELETE FROM patients_basic_data WHERE id={};", id);

        // Execute the query using DatabaseHandler
        json query_results_json = dbHandler.executeQuery(query);

        evaluate_response(response_json, query_results_json);
        finish_response(res, 200, response_json);

    } catch (const std::exception& e) {
        // Handle exception (log, etc.)
        format_response(response_json, -2, "failure", fmt::format("failed: {}", e.what()));
        finish_response(res, 500, response_json);
    }
}

void RestHandler::register_user(const crow::request& req, crow::response& res)
{
    json response_json;
    // try to parse json and throw error if invalid json
    try {
        auto userdata_json = json::parse(req.body);
    } catch (const std::exception& e) {
        format_response(response_json, -1, "failed to create a new user, invalid json", fmt::format("error parsing user data: {}", e.what()));
        finish_response(res, 1001, response_json);
        return;
        ;
    }
    // parse the json and extract the data
    try {
        auto userdata_json = json::parse(req.body);

        std::string username = userdata_json["username"].as<std::string>();
        userdata_json.erase("username");
        std::string password = userdata_json["password"].as<std::string>();

        // Define the regex pattern for a valid password
        const std::regex password_pattern("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[!@#$%^&*])[A-Za-z\\d!@#$%^&*]{8,}$");

        // Check if the password matches the pattern
        if (!std::regex_match(password, password_pattern)) {
            format_response(response_json, -1, "failed to create a new user, invalid password", "password in weak");
            finish_response(res, 400, response_json);
            return;
        }

        userdata_json.erase("password");
        std::string password_hash = picosha2::hash256_hex_string(password);
        std::string role = userdata_json["role"].as<std::string>();
        std::string user_data = userdata_json["user_data"].as<std::string>();
        std::string email = userdata_json["user_data"]["contact"]["email"].as<std::string>();

        // check if username contains spaces
        std::regex space_pattern("\\s");
        if (std::regex_search(username, space_pattern)) {
            format_response(response_json, -1, "failed to create a new user, username contains spaces", "username contains spaces");
            finish_response(res, 400, response_json);
            return;
        }

        // check if user exists
        if (dbHandler.checkItemExists("users", "username", username)) {
            format_response(response_json, -1, "failed to create a new user, user exists", "user already exists");
            finish_response(res, 400, response_json);
            return;
        }
        // check if username or password or email are empty
        if (username.empty() || password.empty() || password_hash.empty()) {
            format_response(response_json, -1, "failed to create a new user, invalid data", "empty username or password");
            finish_response(res, 400, response_json);
            return;
        }

        // Check if the email matches the pattern
        std::regex email_pattern(R"((\w+)(\.\w+)*@(\w+)(\.\w+)+)");
        if (!std::regex_match(email, email_pattern)) {
            format_response(response_json, -1, "failed to create a new user, invalid data", "invalid email format");
            finish_response(res, 400, response_json);
            return;
        }

        //  Construct SQL query using {fmt} for parameterized query
        std::string query
            = fmt::format("INSERT INTO users (username, password_hash, role, user_data) VALUES ('{}','{}','{}','{}') ",
                username, password_hash, role, user_data);

        // Execute the query using DatabaseHandler
        json query_results_json = dbHandler.executeQuery(query);

        evaluate_response(response_json, query_results_json);
        finish_response(res, 200, response_json);

    } catch (const std::exception& e) {
        // Handle exception (log, etc.)
        format_response(response_json, -2, "failure", fmt::format("failed: {}", e.what()));
        finish_response(res, 500, response_json);
    }
}

uint64_t RestHandler::get_next_patient_id()
{
    json json_nextval = dbHandler.executeQuery("SELECT NEXTVAL('patient_id_seq');");

    // Iterate through each object in the JSON array
    for (const auto& obj : json_nextval.array_range()) {
        if (obj.contains("nextval")) {
            return obj["nextval"].as<uint64_t>();
        }
    }
    return 0;
}
bool RestHandler::check_affected_rows(const json& response)
{
    for (const auto& obj : response.array_range()) {
        if (obj.contains("affected rows")) {
            return obj["affected rows"].as<uint64_t>() == 1;
        }
    }
    return false;
}

void RestHandler::evaluate_response(json& response_json, const json& query_results_json)
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

void RestHandler::finish_response(crow::response& res, const int& code, const json& response_json)
{
    std::string body;
    response_json.dump(body, jsoncons::indenting::indent);
    res.code = code;
    res.write(body);
    res.end();
}