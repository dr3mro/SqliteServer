#include "databaseconnectionpool.hpp"
#include "threadpool.hpp"
#include <chrono>
#include <crow.h>
#include <memory.h>
#include <thread>

#include "databasehandler.hpp"
#include "resthandler.hpp"

const int MAX_RETRIES = 10;
const int RETRY_DELAY_MS = 10000;

template <typename Func>
auto executeWithRetry(Func func) -> decltype(func())
{
    int attempts = 0;
    while (attempts < MAX_RETRIES) {
        try {
            return func();
        } catch (const std::exception& e) {
            if (++attempts >= MAX_RETRIES) {
                throw;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_DELAY_MS));
        }
    }
    throw std::runtime_error("Max retries reached");
}

int main()
{
    try {
        // Initialize thread pool and database connection pool
        ThreadPool threadPool(2);
        DatabaseConnectionPool dbConnPool(4);
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