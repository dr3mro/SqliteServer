#include "usercontroller.hpp"
#include <fmt/core.h> // Include fmt library for string formatting
#include <picosha2.h>
#include <regex>

// Definition of the implementation class
class UserController::Impl {
public:
    Impl(DatabaseController& dbController, RestHelper& rHelper)
        : dbController(dbController)
        , rHelper(rHelper)
    {
    }

    typedef struct {
        std::string username;
        std::string password_hash;
        std::string role;
        std::string user_data;

    } UserRegisterationData;

    void respond_with_error(crow::response& res, json& response_json, const std::string& status_message, const std::string& response, const short status, const short code);
    bool is_request_data_valid(const crow::request& req, crow::response& res, json& response_json);
    inline bool is_username_pattern_valid(const std::string& username);
    inline bool is_string_contains_spaces(const std::string& string);
    inline bool is_password_pattern_valid(const std::string& password);
    inline bool is_email_pattern_valid(const std::string& email);
    inline bool extract_and_sanity_check_user_registeration_data(UserRegisterationData& userRegisterationData, json& userdata_json, json& response_json, crow::response& res);
    DatabaseController& dbController;
    RestHelper& rHelper;
};
void UserController::Impl::respond_with_error(crow::response& res, json& response_json, const std::string& status_message, const std::string& response, const short status, const short code)
{
    rHelper.format_response(response_json, status, status_message, response);
    rHelper.finish_response(res, code, response_json);
}

bool UserController::Impl::is_request_data_valid(const crow::request& req, crow::response& res, json& response_json)
{
    // try to parse json and throw error if invalid json
    try {
        auto userdata_json = json::parse(req.body);
        return true;
    } catch (const std::exception& e) {
        respond_with_error(res, response_json, "failed to create a new user, invalid json", fmt::format("error parsing user data: {}", e.what()), -1, 400);
        return false;
    }
}

inline bool UserController::Impl::is_username_pattern_valid(const std::string& username)
{
    const std::regex pattern("^[a-z][a-z0-9_]*$");
    return std::regex_match(username, pattern);
}
inline bool UserController::Impl::is_string_contains_spaces(const std::string& string)
{
    std::regex pattern("\\s");
    return std::regex_search(string, pattern);
}

inline bool UserController::Impl::is_password_pattern_valid(const std::string& password)
{
    const std::regex pattern("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[!@#$%^&*])[A-Za-z\\d!@#$%^&*]{8,}$");
    return std::regex_match(password, pattern);
}

inline bool UserController::Impl::is_email_pattern_valid(const std::string& email)
{
    const std::regex pattern(R"((\w+)(\.\w+)*@(\w+)(\.\w+)+)");
    return std::regex_match(email, pattern);
}

inline bool UserController::Impl::extract_and_sanity_check_user_registeration_data(UserRegisterationData& userRegisterationData, json& userdata_json, json& response_json, crow::response& res)
{
    // extract the username from the json
    userRegisterationData.username = userdata_json["username"].as<std::string>();

    // check username pattern
    if (is_username_pattern_valid(userRegisterationData.username)) {
        userdata_json.erase("username");
    } else {
        respond_with_error(res, response_json, "failed to create a new user, invalid username", "username should always be in lowercase characters and underscore or numbers only", -1, 400);
        return false;
    }

    // check password pattern
    std::string password = userdata_json["password"].as<std::string>();

    if (is_password_pattern_valid(password)) {
        userdata_json.erase("password");
    } else {
        respond_with_error(res, response_json, "failed to create a new user, invalid password", "password is weak", -1, 400);
        return false;
    }

    // create the password hash
    userRegisterationData.password_hash = picosha2::hash256_hex_string(password);

    // extract the role, userdata and email for further validation.
    userRegisterationData.role = userdata_json["role"].as<std::string>();
    userRegisterationData.user_data = userdata_json["user_data"].as<std::string>();

    std::string email = userdata_json["user_data"]["contact"]["email"].as<std::string>();

    // check if username contains spaces.
    if (is_string_contains_spaces(userRegisterationData.username)) {
        respond_with_error(res, response_json, "failed to create a new user, username contains spaces", "username contains spaces", -1, 400);
        return false;
    }

    // check if user exists
    if (dbController.checkItemExists("users", "username", userRegisterationData.username)) {
        respond_with_error(res, response_json, "failed to create a new user, user exists", "user already exists", -1, 400);
        return false;
    }

    // check if username or password or email are empty
    if (userRegisterationData.username.empty() || password.empty() || userRegisterationData.password_hash.empty()) {
        respond_with_error(res, response_json, "failed to create a new user, invalid data", "empty username or password", -1, 400);
        return false;
    }

    // Check if the email matches the pattern
    if (!is_email_pattern_valid(email)) {
        respond_with_error(res, response_json, "failed to create a new user, invalid data", "invalid email format", -1, 400);
        return false;
    }
    return true;
}

UserController::UserController(DatabaseController& dbController, RestHelper& rHelper)
    : pImpl(std::make_unique<Impl>(dbController, rHelper))
{
}
void UserController::register_user(const crow::request& req, crow::response& res)
{
    json response_json;
    // check json validity
    if (!pImpl->is_request_data_valid(req, res, response_json)) {
        return;
    }

    // parse the json and extract the data
    try {
        // get the json from request body
        auto userdata_json = json::parse(req.body);

        Impl::UserRegisterationData userRegisterationData;

        // extract and validate user registeration data
        if (!pImpl->extract_and_sanity_check_user_registeration_data(userRegisterationData, userdata_json, response_json, res)) {
            return;
        }

        //  Construct SQL query using {fmt} for parameterized query
        std::string query
            = fmt::format("INSERT INTO users (username, password_hash, role, user_data) VALUES ('{}','{}','{}','{}') ",
                userRegisterationData.username, userRegisterationData.password_hash, userRegisterationData.role, userRegisterationData.user_data);

        // Execute the query using DatabaseHandler
        json query_results_json = pImpl->dbController.executeQuery(query);

        pImpl->rHelper.evaluate_response(response_json, query_results_json);
        pImpl->rHelper.finish_response(res, 200, response_json);

    } catch (const std::exception& e) {
        // Handle exception (log, etc.)
        pImpl->respond_with_error(res, response_json, "failure", fmt::format("failed: {}", e.what()), -2, 500);
    }
}
