#include "databaseconnectionpool.hpp"
#include "threadpool.hpp"
#include <chrono>
#include <crow.h>
#include <thread>

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

    ThreadPool threadPool(256);
    DatabaseConnectionPool dbConnPool(256);

    RestHandler rest_handler(dbConnPool);

    crow::SimpleApp app;

    // Define routes
    CROW_ROUTE(app, "/get")
        .name("get")
        .methods("GET"_method)([&](const crow::request& req, crow::response& res) {
            rest_handler.handle_get(req, res);
        });

    CROW_ROUTE(app, "/post")
        .name("post")
        .methods("POST"_method)([&](const crow::request& req, crow::response& res) {
            rest_handler.handle_post(req, res);
        });

    // Start the server on port 8080
    app.port(8080).multithreaded().run();

    return 0;
}
