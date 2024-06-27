#include "databaseconnectionpool.hpp"
#include "databasecontroller.hpp"
#include "restcontroller.hpp"
#include "resthelper.hpp"
#include <crow.h>
#include <fmt/core.h>
#include <memory.h>
#include <thread>

#define PORT 8080

int main()
{
    try {
        // Get the number of cpu cores of CPU to define the number of threads for the server and threadpool
        unsigned int ncpus = std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 1;

        // initialize database connections pool
        DatabaseConnectionPool dbConnPool(ncpus * 3);

        // create the db controller and handle it the connection
        DatabaseController dbController(dbConnPool);

        // Create REST helper
        RestHelper restHelper(dbController);

        // Create REST handler
        RestController restController(dbController, restHelper);

        // Initialize Crow application
        crow::SimpleApp app;

        ////////////////   ROUTES   ////////////////////
        CROW_ROUTE(app, "/v1/register")
            .methods("POST"_method)([&restController](const crow::request& req, crow::response& res) {
                restController.register_user(std::ref(req), std::ref(res));
            });

        CROW_ROUTE(app, "/v1/authenticate")
            .methods("POST"_method)([&restController](const crow::request& req, crow::response& res) {
                // here we implement code to check login credenials and create token to be used during the next requests
                // it well get user and password in header and return a token that will be stored in the database for later use
            });

        CROW_ROUTE(app, "/v1/retrieve")
            .methods("GET"_method)([&restController](const crow::request& req, crow::response& res) {
                // here we will check the header for token and if valid the check for a variable that specify
                // the column and the id needed and it will be returned as json list of data from database
                //
            });

        CROW_ROUTE(app, "/v1/store")
            .methods("POST"_method)([&restController](const crow::request& req, crow::response& res) {
                // here we will check the header for token and if valid the check for a variable that specify
                // the operation either create or update then we check if the id exsists and check the column and
                // the id needed and it will be store the json list of data into database and if the variable = delete
                // then we check the user role and delete patient
                //
            });
        CROW_ROUTE(app, "/v1/store")
            .methods("PUT"_method)([&restController](const crow::request& req, crow::response& res) {
                // here we will check the header for token and if valid the check for a variable that specify
                // the operation either create or update then we check if the id exsists and check the column and
                // the id needed and it will be store the json list of data into database and if the variable = delete
                // then we check the user role and delete patient
                //
            });

        CROW_ROUTE(app, "/v1/store")
            .methods("DELETE"_method)([&restController](const crow::request& req, crow::response& res) {
                // here we will check the header for token and if valid the check for a variable that specify
                // the operation either create or update then we check if the id exsists and check the column and
                // the id needed and it will be store the json list of data into database and if the variable = delete
                // then we check the user role and delete patient
                //
            });

        CROW_ROUTE(app, "/v1/search")
            .methods("GET"_method)([&restController](const crow::request& req, crow::response& res) {
                // here we will check the header for token and if valid the check for a query string in the database and return
                // list of results as json .. it will be expensive
            });

        ///////////////

        CROW_ROUTE(app, "/api_v1/create_patient_basic_information")
            .methods("POST"_method)([&restController](const crow::request& req, crow::response& res) {
                restController.create_patient_basic_information(std::ref(req), std::ref(res));
            });
        // GET route example: /get/<int>
        CROW_ROUTE(app, "/api_v1/read_patient_basic_information/<int>")
            .methods("GET"_method)([&restController](const crow::request& req, crow::response& res, uint64_t id) {
                restController.read_patient_basic_information(std::ref(req), std::ref(res), id);
            });

        CROW_ROUTE(app, "/api_v1/update_patient_basic_information/<int>")
            .methods("POST"_method)([&restController](const crow::request& req, crow::response& res, uint64_t id) {
                restController.update_patient_basic_information(std::ref(req), std::ref(res), id);
            });

        CROW_ROUTE(app, "/api_v1/delete_patient_basic_information/<int>")
            .methods("GET"_method)([&restController](const crow::request& req, crow::response& res, uint64_t id) {
                restController.delete_patient_basic_information(std::ref(req), std::ref(res), id);
            });

        CROW_CATCHALL_ROUTE(app)
        ([](crow::response& res) {
            res.code = crow::OK;
            res.end("Welcome to the ___ ASGARD ___.");
        });

        // Start the server on port 8080
        std::cout << fmt::format("database server is started on port {}.\n", PORT);
        app.loglevel(crow::LogLevel::INFO)
            .use_compression(crow::compression::algorithm::GZIP)
            .port(PORT)
            .multithreaded()
            .concurrency(ncpus * 4)
            .bindaddr("0.0.0.0")
            .server_name("ProjectValhalla")
            .run();
    } catch (const std::exception& e) {
        std::cerr << "Exception caught in main: " << e.what() << std::endl;
        return 1; // Exit with error code
    }

    return 0;
}