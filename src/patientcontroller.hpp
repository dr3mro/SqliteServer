#include "databasecontroller.hpp"
#include "resthelper.hpp"
#include <crow.h>

using json = jsoncons::json;

class PatientController {
public:
    PatientController(DatabaseController& dbController, RestHelper& rHelper);

    // CRUD
    void create_patient_basic_information(const crow::request& req, crow::response& res);
    void read_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id);
    void update_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id);
    void delete_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id);

    void register_user(const crow::request& req, crow::response& res);

private:
    DatabaseController& dbController;
    RestHelper& rHelper;
};
