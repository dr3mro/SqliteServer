#include "databaseconnectionpool.hpp"
#include "threadpool.hpp"
#include <crow.h>
#include <memory.h>
#include <thread>

#include "databasehandler.hpp"
#include "resthandler.hpp"

int main()
{
    try {

        unsigned int ncpus = std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 1;

        // Initialize thread pool and database connection pool
        ThreadPool threadPool(ncpus * 1.5);
        DatabaseConnectionPool dbConnPool(ncpus * 2);

        DatabaseHandler dbHandler(dbConnPool);

        // Create REST handler
        RestHandler restHandler(dbHandler, threadPool);

        // Initialize Crow application
        crow::SimpleApp app;

        // GET route example: /get/<int>
        CROW_ROUTE(app, "/get/<int>")
            .methods("GET"_method)([&restHandler](const crow::request& req, crow::response& res, int id) {
                restHandler.handle_get(req, res, id);
            });

        CROW_ROUTE(app, "/api_v1/create_patient/")
            .name("create_patient")
            .methods("POST"_method)([&restHandler](const crow::request& req, crow::response& res) {
                restHandler.handle_create_client_personal_history(req, res);
            });

        // Start the server on port 8080
        app.loglevel(crow::LogLevel::Critical);
        app.port(8080).multithreaded().run();
    } catch (const std::exception& e) {
        std::cerr << "Exception caught in main: " << e.what() << std::endl;
        return 1; // Exit with error code
    }

    return 0;
}