#pragma once
#include "databasecontroller.hpp"
#include "resthelper.hpp"
#include "tokenizer.hpp"
#include <crow.h>

using json = jsoncons::json;

class PatientController {
public:
    PatientController(DatabaseController& dbController, RestHelper& rHelper, Tokenizer& tokenizer);

    // CRUDS
    void create_patient(const crow::request& req, crow::response& res);
    void read_patient(const crow::request& req, crow::response& res);
    void update_patient(const crow::request& req, crow::response& res);
    void delete_patient(const crow::request& req, crow::response& res);
    void search_patient(const crow::request& req, crow::response& res);

private:
    DatabaseController& dbController;
    RestHelper& rHelper;
    Tokenizer& tokenizer;
};
