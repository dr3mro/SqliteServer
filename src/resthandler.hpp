#include "databasehandler.hpp"
#include <crow.h>

using json = nlohmann::json;

class RestHandler {
public:
    RestHandler(DatabaseHandler& dbHandler);

    // CRUD
    void create_patient_basic_information(const crow::request& req, crow::response& res);
    void read_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id);
    void update_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id);
    void delete_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id);

private:
    DatabaseHandler& dbHandler;

    // assistive methods
    uint64_t get_next_patient_id();
    bool check_affected_rows(const json& response);
    void evaluate_response(json& response_json, const json& query_results_json);
    void format_response(json& response_json, const short status, const std::string& status_message, const json& response);
    void finish_response(crow::response& res, const int& code, const std::string& body);
};
