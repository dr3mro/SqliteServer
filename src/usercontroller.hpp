#pragma once

#include "databasecontroller.hpp"
#include "resthelper.hpp"
#include <crow.h>
#include <memory>

using json = jsoncons::json;

class UserController {
public:
    UserController(DatabaseController& dbController, RestHelper& rHelper);
    void register_user(const crow::request& req, crow::response& res);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};
