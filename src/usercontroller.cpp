#include "usercontroller.hpp"
#include <fmt/core.h> // Include fmt library for string formatting
#include <picosha2.h>
#include <regex>

UserController::UserController(DatabaseController& dbController, RestHelper& rHelper)
    : dbController(dbController)
    , rHelper(rHelper)
{
}
void UserController::register_user(const crow::request& req, crow::response& res)
{
    json response_json;
    // try to parse json and throw error if invalid json
    try {
        auto userdata_json = json::parse(req.body);
    } catch (const std::exception& e) {
        error(res, response_json, "failed to create a new user, invalid json", fmt::format("error parsing user data: {}", e.what()), -1, 400);
        return;
    }
    // parse the json and extract the data
    try {
        auto userdata_json = json::parse(req.body);

        std::string username = userdata_json["username"].as<std::string>();
        const std::regex username_pattern("^[a-z][a-z0-9]*$");

        // Check if the string matches the pattern
        if (!std::regex_match(username, username_pattern)) {
            error(res, response_json, "failed to create a new user, invalid username", "username should always be in lowercase characters and numbers only", -1, 400);
            return;
        }

        userdata_json.erase("username");
        std::string password = userdata_json["password"].as<std::string>();

        // Define the regex pattern for a valid password
        const std::regex password_pattern("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[!@#$%^&*])[A-Za-z\\d!@#$%^&*]{8,}$");

        // Check if the password matches the pattern
        if (!std::regex_match(password, password_pattern)) {
            error(res, response_json, "failed to create a new user, invalid password", "password is weak", -1, 400);
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
            error(res, response_json, "failed to create a new user, username contains spaces", "username contains spaces", -1, 400);
            return;
        }

        // check if user exists
        if (dbController.checkItemExists("users", "username", username)) {
            error(res, response_json, "failed to create a new user, user exists", "user already exists", -1, 400);
            return;
        }

        // check if username or password or email are empty
        if (username.empty() || password.empty() || password_hash.empty()) {
            error(res, response_json, "failed to create a new user, invalid data", "empty username or password", -1, 400);
            return;
        }

        // Check if the email matches the pattern
        std::regex email_pattern(R"((\w+)(\.\w+)*@(\w+)(\.\w+)+)");
        if (!std::regex_match(email, email_pattern)) {
            error(res, response_json, "failed to create a new user, invalid data", "invalid email format", -1, 400);
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
        error(res, response_json, "failure", fmt::format("failed: {}", e.what()), -2, 500);
    }
}

void UserController::error(crow::response& res, json& response_json, const std::string& status_message, const std::string& response, const short status, const short code)
{
    rHelper.format_response(response_json, status, status_message, response);
    rHelper.finish_response(res, code, response_json);
}