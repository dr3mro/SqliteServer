#pragma once
#include "databasecontroller.hpp"
#include <crow.h>
#include <jsoncons/json.hpp>

using json = jsoncons::json;

class RestHelper {
public:
    RestHelper(DatabaseController& dbController);

    uint64_t get_next_patient_id();
    bool check_affected_rows(const json& response);
    void evaluate_response(json& response_json, const json& query_results_json);
    void format_response(json& response_json, const short status, const std::string& status_message, const json& response);
    void finish_response(crow::response& res, const int& code, const json& response_json);

private:
    DatabaseController& dbController;
};
