#include "databaseconnectionpool.hpp"
#include "threadpool.hpp"
#include <chrono>
#include <crow.h>
#include <memory.h>
#include <thread>

#include "databasehandler.hpp"
#include "resthandler.hpp"

int main()
{
    try {
        // Initialize thread pool and database connection pool
        ThreadPool threadPool(64);
        DatabaseConnectionPool dbConnPool(32);
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

        CROW_ROUTE(app, "/post")
            .name("post")
            .methods("POST"_method)([&restHandler](const crow::request& req, crow::response& res) {
                restHandler.handle_post(req, res);
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