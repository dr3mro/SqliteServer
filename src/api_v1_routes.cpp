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
            // here we will check the header for token and if valid the check for a variable that specify
            // the column and the id needed and it will be returned as json list of data from database
            //
        });

    CROW_ROUTE(app, "/v1/store")
        .methods("POST"_method)([&patientController](const crow::request& req, crow::response& res) {
            // here we will check the header for token and if valid the check for a variable that specify
            // the operation either create or update then we check if the id exsists and check the column and
            // the id needed and it will be store the json list of data into database and if the variable = delete
            // then we check the user role and delete patient
            //
        });
    CROW_ROUTE(app, "/v1/store")
        .methods("PUT"_method)([&patientController](const crow::request& req, crow::response& res) {
            // here we will check the header for token and if valid the check for a variable that specify
            // the operation either create or update then we check if the id exsists and check the column and
            // the id needed and it will be store the json list of data into database and if the variable = delete
            // then we check the user role and delete patient
            //
        });

    CROW_ROUTE(app, "/v1/store")
        .methods("DELETE"_method)([&patientController](const crow::request& req, crow::response& res) {
            // here we will check the header for token and if valid the check for a variable that specify
            // the operation either create or update then we check if the id exsists and check the column and
            // the id needed and it will be store the json list of data into database and if the variable = delete
            // then we check the user role and delete patient
            //
        });

    CROW_ROUTE(app, "/v1/search")
        .methods("GET"_method)([&patientController](const crow::request& req, crow::response& res) {
            // here we will check the header for token and if valid the check for a query string in the database and return
            // list of results as json .. it will be expensive
        });

    ///////////////

    CROW_ROUTE(app, "/api_v1/create_patient_basic_information")
        .methods("POST"_method)([&patientController](const crow::request& req, crow::response& res) {
            patientController.create_patient_basic_information(std::ref(req), std::ref(res));
        });
    // GET route example: /get/<int>
    CROW_ROUTE(app, "/api_v1/read_patient_basic_information/<int>")
        .methods("GET"_method)([&patientController](const crow::request& req, crow::response& res, uint64_t id) {
            patientController.read_patient_basic_information(std::ref(req), std::ref(res), id);
        });

    CROW_ROUTE(app, "/api_v1/update_patient_basic_information/<int>")
        .methods("POST"_method)([&patientController](const crow::request& req, crow::response& res, uint64_t id) {
            patientController.update_patient_basic_information(std::ref(req), std::ref(res), id);
        });

    CROW_ROUTE(app, "/api_v1/delete_patient_basic_information/<int>")
        .methods("GET"_method)([&patientController](const crow::request& req, crow::response& res, uint64_t id) {
            patientController.delete_patient_basic_information(std::ref(req), std::ref(res), id);
        });

    CROW_CATCHALL_ROUTE(app)
    ([](crow::response& res) {
        res.code = crow::OK;
        res.end("Welcome to ___ ASGARD ___.");
    });
}
