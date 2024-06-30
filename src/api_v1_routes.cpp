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
            patientController.create_patient(std::ref(req), std::ref(res));
        });

    CROW_ROUTE(app, "/v1/patient")
        .methods(crow::HTTPMethod::GET)([&patientController](const crow::request& req, crow::response& res) {
            patientController.read_patient(std::ref(req), std::ref(res));
        });

    CROW_ROUTE(app, "/v1/patient")
        .methods(crow::HTTPMethod::PATCH)([&patientController](const crow::request& req, crow::response& res) {
            patientController.update_patient(std::ref(req), std::ref(res));
        });

    CROW_ROUTE(app, "/v1/patient")
        .methods(crow::HTTPMethod::DELETE)([&patientController](const crow::request& req, crow::response& res) {
            patientController.delete_patient(std::ref(req), std::ref(res));
        });

    CROW_ROUTE(app, "/v1/patient")
        .methods(crow::HTTPMethod::SEARCH)([&patientController](const crow::request& req, crow::response& res) {
            patientController.search_patient(std::ref(req), std::ref(res));
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
