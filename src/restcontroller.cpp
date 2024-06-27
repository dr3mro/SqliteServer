#include "restcontroller.hpp"
#include <crow.h>
#include <fmt/core.h> // Include fmt library for string formatting
#include <picosha2.h>
#include <regex>

RestController::RestController(DatabaseController& dbController, RestHelper& rHelper)
    : dbController(dbController)
    , rHelper(rHelper)
{
}

void RestController::create_patient_basic_information(const crow::request& req, crow::response& res)
{
    json response_json;
    auto basic_data_json = json::parse(req.body);

    try {
        uint64_t nextid = rHelper.get_next_patient_id();

        if (nextid == 0) {
            rHelper.format_response(response_json, -1, "failed to create a new patient", "failed to get nextval");
            rHelper.finish_response(res, 401, response_json);
        }

        basic_data_json["id"] = nextid;
        // Construct SQL query using {fmt} for parameterized query
        std::string query
            = fmt::format("INSERT INTO patients_basic_data (id, basic_data) VALUES ('{}','{}') RETURNING basic_data;",
                nextid, basic_data_json.to_string());

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

void RestController::read_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id)
{
    json response_json;
    try {
        std::string query
            = fmt::format("SELECT basic_data FROM patients_basic_data WHERE id = {}", id);
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

void RestController::update_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id)
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
void RestController::delete_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id)
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

void RestController::register_user(const crow::request& req, crow::response& res)
{
    json response_json;
    // try to parse json and throw error if invalid json
    try {
        auto userdata_json = json::parse(req.body);
    } catch (const std::exception& e) {
        rHelper.format_response(response_json, -1, "failed to create a new user, invalid json", fmt::format("error parsing user data: {}", e.what()));
        rHelper.finish_response(res, 1001, response_json);
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
            rHelper.format_response(response_json, -1, "failed to create a new user, invalid password", "password in weak");
            rHelper.finish_response(res, 400, response_json);
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
            rHelper.format_response(response_json, -1, "failed to create a new user, username contains spaces", "username contains spaces");
            rHelper.finish_response(res, 400, response_json);
            return;
        }

        // check if user exists
        if (dbController.checkItemExists("users", "username", username)) {
            rHelper.format_response(response_json, -1, "failed to create a new user, user exists", "user already exists");
            rHelper.finish_response(res, 400, response_json);
            return;
        }
        // check if username or password or email are empty
        if (username.empty() || password.empty() || password_hash.empty()) {
            rHelper.format_response(response_json, -1, "failed to create a new user, invalid data", "empty username or password");
            rHelper.finish_response(res, 400, response_json);
            return;
        }

        // Check if the email matches the pattern
        std::regex email_pattern(R"((\w+)(\.\w+)*@(\w+)(\.\w+)+)");
        if (!std::regex_match(email, email_pattern)) {
            rHelper.format_response(response_json, -1, "failed to create a new user, invalid data", "invalid email format");
            rHelper.finish_response(res, 400, response_json);
            return;
        }

        //  Construct SQL query using {fmt} for parameterized query
        std::string query
            = fmt::format("INSERT INTO users (username, password_hash, role, user_data) VALUES ('{}','{}','{}','{}') ",
                username, password_hash, role, user_data);

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
