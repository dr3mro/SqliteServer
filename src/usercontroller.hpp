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
};
