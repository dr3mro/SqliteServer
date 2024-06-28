#pragma once

#include "databasecontroller.hpp"
#include "resthelper.hpp"
#include <crow.h>

using json = jsoncons::json;

class UserController {
public:
    UserController(DatabaseController& dbController, RestHelper& rHelper);
    void register_user(const crow::request& req, crow::response& res);

private:
    DatabaseController& dbController;
    RestHelper& rHelper;

    typedef struct {
        std::string username;
        std::string password_hash;
        std::string role;
        std::string user_data;
        std::string email;

    } UserRegisterationData;

    void respond_with_error(crow::response& res, json& response_json, const std::string& status_message, const std::string& response, const short status, const short code);
    bool is_request_data_valid(const crow::request& req, crow::response& res, json& response_json);
    inline bool is_username_pattern_valid(const std::string& username);
    inline bool is_string_contains_spaces(const std::string& string);
    inline bool is_password_pattern_valid(const std::string& password);
    inline bool is_email_pattern_valid(const std::string& email);
    inline bool extract_and_sanity_check_user_registeration_data(UserRegisterationData& userRegisterationData, json& userdata_json, json& response_json, crow::response& res);
};
