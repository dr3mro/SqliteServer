#include "api_v1_routes.hpp"

API_V1_Routes::API_V1_Routes(crow::SimpleApp& app, UserController& userController, PatientController& patientController)
    : app(app)
    , userController(userController)
    , patientController(patientController)
{
    ////////////////   ROUTES   ////////////////////
    CROW_ROUTE(app, "/v1/register")
        .methods("POST"_method)([&userController](const crow::request& req, crow::response& res) {
            userController.register_user(std::ref(req), std::ref(res));
        });

    CROW_ROUTE(app, "/v1/login")
        .methods("POST"_method)([&userController](const crow::request& req, crow::response& res) {
            userController.login_user(std::ref(req), std::ref(res));
        });

    CROW_ROUTE(app, "/v1/retrieve")
        .methods("GET"_method)([&patientController](const crow::request& req, crow::response& res) {
            patientController.get_patient_data(std::ref(req), std::ref(res));
        });

    CROW_ROUTE(app, "/v1/store")
        .methods("POST"_method)([&patientController](const crow::request& req, crow::response& res) {
            patientController.create_new_patient(std::ref(req), std::ref(res));
        });
    CROW_ROUTE(app, "/v1/store")
        .methods("PATCH"_method)([&patientController](const crow::request& req, crow::response& res) {
            // here we will check the header for token and if valid the check for a variable that specify
            // the operation either create or update then we check if the id exsists and check the column and
            // the id needed and it will be store the json list of data into database and if the variable = delete
            // then we check the user role and delete patient
            //
        });

    CROW_ROUTE(app, "/v1/store")
        .methods("DELETE"_method)([&patientController](const crow::request& req, crow::response& res) {
            patientController.delete_patient(std::ref(req), std::ref(res));
        });

    CROW_ROUTE(app, "/v1/search")
        .methods("GET"_method)([&patientController](const crow::request& req, crow::response& res) {
            // here we will check the header for token and if valid the check for a query string in the database and return
            // list of results as json .. it will be expensive
        });

    CROW_CATCHALL_ROUTE(app)
    ([](crow::response& res) {
        res.code = crow::NOT_FOUND;
        res.end("NOT FOUND, Are you lost?");
    });
}
