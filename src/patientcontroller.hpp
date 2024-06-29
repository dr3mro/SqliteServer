#pragma once
#include "databasecontroller.hpp"
#include "resthelper.hpp"
#include "tokenizer.hpp"
#include <crow.h>

using json = jsoncons::json;

class PatientController {
public:
    PatientController(DatabaseController& dbController, RestHelper& rHelper, Tokenizer& tokenizer);

    void create_new_patient(const crow::request& req, crow::response& res);
    void get_patient_data(const crow::request& req, crow::response& res);

    void update_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id);
    void delete_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id);

private:
    DatabaseController& dbController;
    RestHelper& rHelper;
    Tokenizer& tokenizer;
};
