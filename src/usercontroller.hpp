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

    void error(crow::response& res, json& response_json, const std::string& status_message, const std::string& response, const short status, const short code);
};
