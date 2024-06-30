#include "api_v1_routes.hpp"

API_V1_Routes::API_V1_Routes(crow::SimpleApp& app, UserController& userController, PatientController& patientController)
    : app(app)
    , userController(userController)
    , patientController(patientController)
{
    ////////////////   ROUTES   ////////////////////
    CROW_ROUTE(app, "/v1/user")
        .methods(crow::HTTPMethod::POST)([&userController](const crow::request& req, crow::response& res) {
            userController.register_user(std::ref(req), std::ref(res));
        });

    CROW_ROUTE(app, "/v1/user")
        .methods(crow::HTTPMethod::GET)([&userController](const crow::request& req, crow::response& res) {
            userController.login_user(std::ref(req), std::ref(res));
        });

    CROW_ROUTE(app, "/v1/patient")
        .methods(crow::HTTPMethod::POST)([&patientController](const crow::request& req, crow::response& res) {
            patientController.create_new_patient(std::ref(req), std::ref(res));
        });

    CROW_ROUTE(app, "/v1/patient")
        .methods(crow::HTTPMethod::GET)([&patientController](const crow::request& req, crow::response& res) {
            patientController.get_patient_data(std::ref(req), std::ref(res));
        });

    CROW_ROUTE(app, "/v1/patient")
        .methods(crow::HTTPMethod::PATCH)([&patientController](const crow::request& req, crow::response& res) {
            // here we will check the header for token and if valid the check for a variable that specify
            // the operation either create or update then we check if the id exsists and check the column and
            // the id needed and it will be store the json list of data into database and if the variable = delete
            // then we check the user role and delete patient
            //
        });

    CROW_ROUTE(app, "/v1/patient")
        .methods(crow::HTTPMethod::DELETE)([&patientController](const crow::request& req, crow::response& res) {
            patientController.delete_patient(std::ref(req), std::ref(res));
        });

    CROW_ROUTE(app, "/v1/patient")
        .methods(crow::HTTPMethod::SEARCH)([&patientController](const crow::request& req, crow::response& res) {
            // here we will check the header for token and if valid the check for a query string in the database and return
            // list of results as json .. it will be expensive
        });

    CROW_ROUTE(app, "/v1/hello")
        .methods(crow::HTTPMethod::GET)([](const crow::request& req, crow::response& res) {
            res.code = crow::OK;
            res.end("Welcome to ASGARD.");
        });

    CROW_CATCHALL_ROUTE(app)
    ([](crow::response& res) {
        res.code = crow::NOT_FOUND;
        res.end("NOT FOUND, Are you lost?");
    });
}
