#include "usercontroller.hpp"
#include <fmt/core.h> // Include fmt library for string formatting
#include <jsoncons/json.hpp> // Include jsoncons library for JSON handling
#include <picosha2.h>
#include <regex>

// Definition of the implementation class
class UserController::Impl {
public:
    Impl(DatabaseController& dbController, RestHelper& rHelper, Tokenizer& tokenizer)
        : dbController(dbController)
        , rHelper(rHelper)
        , tokenizer(tokenizer)
    {
    }

    typedef struct {
        std::string username;
        std::string password_hash;
        std::string role;
        std::string user_data;
    } UserRegistrationData;

    bool is_username_pattern_valid(const std::string& username);
    bool is_string_contains_spaces(const std::string& string);
    bool is_password_pattern_valid(const std::string& password);
    bool is_email_pattern_valid(const std::string& email);
    bool extract_and_sanity_check_user_registration_data(UserRegistrationData& userRegistrationData, json& userdata_json, json& response_json, crow::response& res);

    uint64_t authenticate_user(const std::string& username, const std::string& password);

    DatabaseController& dbController;
    RestHelper& rHelper;
    Tokenizer& tokenizer;
};

bool UserController::Impl::is_username_pattern_valid(const std::string& username)
{
    const std::regex pattern("^[a-z][a-z0-9_]*$");
    return std::regex_match(username, pattern);
}

bool UserController::Impl::is_string_contains_spaces(const std::string& string)
{
    std::regex pattern("\\s");
    return std::regex_search(string, pattern);
}

bool UserController::Impl::is_password_pattern_valid(const std::string& password)
{
    const std::regex pattern("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[!@#$%^&*])[A-Za-z\\d!@#$%^&*]{8,}$");
    return std::regex_match(password, pattern);
}

bool UserController::Impl::is_email_pattern_valid(const std::string& email)
{
    const std::regex pattern(R"((\w+)(\.\w+)*@(\w+)(\.\w+)+)");
    return std::regex_match(email, pattern);
}

bool UserController::Impl::extract_and_sanity_check_user_registration_data(UserRegistrationData& userRegistrationData, json& userdata_json, json& response_json, crow::response& res)
{
    // Extract the username from the JSON
    userRegistrationData.username = userdata_json["username"].as<std::string>();

    // Check username pattern
    if (is_username_pattern_valid(userRegistrationData.username)) {
        userdata_json.erase("username");
    } else {
        rHelper.respond_with_error(res, response_json, "Failed to create a new user, invalid username", "Username should always be in lowercase characters and underscore or numbers only", -1, 400);
        return false;
    }

    // Check password pattern
    std::string password = userdata_json["password"].as<std::string>();

    if (is_password_pattern_valid(password)) {
        userdata_json.erase("password");
    } else {
        rHelper.respond_with_error(res, response_json, "Failed to create a new user, invalid password", "Password is weak", -1, 400);
        return false;
    }

    // Create the password hash
    userRegistrationData.password_hash = picosha2::hash256_hex_string(password);

    // Extract the role, user_data, and email for further validation
    userRegistrationData.role = userdata_json["role"].as<std::string>();
    userRegistrationData.user_data = userdata_json["user_data"].as<std::string>();

    std::string email = userdata_json["user_data"]["contact"]["email"].as<std::string>();

    // Check if username contains spaces
    if (is_string_contains_spaces(userRegistrationData.username)) {
        rHelper.respond_with_error(res, response_json, "Failed to create a new user, username contains spaces", "Username contains spaces", -1, 400);
        return false;
    }

    // Check if user exists
    if (dbController.checkItemExists("users", "username", userRegistrationData.username)) {
        rHelper.respond_with_error(res, response_json, "Failed to create a new user, user exists", "User already exists", -1, 400);
        return false;
    }

    // Check if username, password, or email are empty
    if (userRegistrationData.username.empty() || password.empty() || userRegistrationData.password_hash.empty()) {
        rHelper.respond_with_error(res, response_json, "Failed to create a new user, invalid data", "Empty username or password", -1, 400);
        return false;
    }

    // Check if the email matches the pattern
    if (!is_email_pattern_valid(email)) {
        rHelper.respond_with_error(res, response_json, "Failed to create a new user, invalid data", "Invalid email format", -1, 400);
        return false;
    }
    return true;
}

uint64_t UserController::Impl::authenticate_user(const std::string& username, const std::string& password)
{
    try {
        uint64_t user_id = dbController.findIfUserID(username);

        if (user_id == 0)
            return 0;

        if (dbController.getPasswordHashForUserID(user_id) == picosha2::hash256_hex_string(password))
            return user_id;
        else
            return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error authenticating user : " << e.what() << std::endl;
    }
    return 0;
}

UserController::UserController(DatabaseController& dbController, RestHelper& rHelper, Tokenizer& tokenizer)
    : pImpl(std::make_unique<Impl>(dbController, rHelper, tokenizer))
{
}

UserController::UserController()
{
}

UserController::~UserController()
{
}

//////////////// PUBLIC /////////////////////////////

void UserController::register_user(const crow::request& req, crow::response& res)
{
    json response_json;
    // Check JSON validity
    if (!pImpl->rHelper.is_request_data_valid(req, res, response_json)) {
        return;
    }

    // Parse the JSON and extract the data
    try {
        // Get the JSON from request body
        auto userdata_json = json::parse(req.body);

        Impl::UserRegistrationData userRegistrationData;

        // Extract and validate user registration data
        if (!pImpl->extract_and_sanity_check_user_registration_data(userRegistrationData, userdata_json, response_json, res)) {
            return;
        }

        // Construct SQL query using {fmt} for parameterized query
        std::string query = fmt::format(
            "INSERT INTO users (username, password_hash, role, user_data) VALUES ('{}','{}','{}','{}')",
            userRegistrationData.username, userRegistrationData.password_hash, userRegistrationData.role, userRegistrationData.user_data);

        // Execute the query using DatabaseController
        json query_results_json = pImpl->dbController.executeQuery(query);

        pImpl->rHelper.evaluate_response(response_json, query_results_json);
        pImpl->rHelper.finish_response(res, 200, response_json);

    } catch (const std::exception& e) {
        // Handle exception (log, etc.)
        pImpl->rHelper.respond_with_error(res, response_json, "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}

void UserController::login_user(const crow::request& req, crow::response& res)
{
    json response_json;
    // Check JSON validity
    if (!pImpl->rHelper.is_request_data_valid(req, res, response_json)) {
        return;
    }

    // Parse the JSON and extract the data
    try {
        // Get the JSON from request body
        auto userdata_json = json::parse(req.body);

        std::string username = userdata_json["username"].as<std::string>();
        std::string password = userdata_json["password"].as<std::string>();

        uint64_t user_id = pImpl->authenticate_user(username, password);

        if (user_id == 0) {
            pImpl->rHelper.respond_with_error(res, response_json, "Login Failure", fmt::format("User '{}' not found or wrong password", username), -1, 400);
            return;
        }

        std::string token = pImpl->tokenizer.generate_token(username);

        if (!pImpl->tokenizer.token_validator(token, username)) {
            pImpl->rHelper.respond_with_error(res, response_json, "Login Failure", fmt::format("Failed to create token for user '{}'", username), -1, 400);
        }

        json token_object;
        token_object["token"] = token;

        pImpl->rHelper.format_response(response_json, 0, "success", token_object);
        pImpl->rHelper.finish_response(res, 200, response_json);

    } catch (const std::exception& e) {
        // Handle exception (log, etc.)
        pImpl->rHelper.respond_with_error(res, response_json, "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}