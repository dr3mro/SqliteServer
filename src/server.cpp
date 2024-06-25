#include "databaseconnectionpool.hpp"
#include "databasehandler.hpp"
#include "resthandler.hpp"
#include "threadpool.hpp"
#include <crow.h>
#include <fmt/core.h>
#include <memory.h>
#include <thread>

#define PORT 8080

int main()
{
    try {

        unsigned int ncpus = std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 1;

        // Initialize thread pool and database connection pool
        ThreadPool threadPool(ncpus * 2);
        DatabaseConnectionPool dbConnPool(ncpus * 3);

        DatabaseHandler dbHandler(dbConnPool);

        // Create REST handler
        RestHandler restHandler(dbHandler);

        // Initialize Crow application
        crow::SimpleApp app;

        CROW_ROUTE(app, "/api_v1/create_patient_basic_information")
            .methods("POST"_method)([&restHandler](const crow::request& req, crow::response& res) {
                restHandler.create_patient_basic_information(std::ref(req), std::ref(res));
            });
        // GET route example: /get/<int>
        CROW_ROUTE(app, "/api_v1/read_patient_basic_information/<int>")
            .methods("GET"_method)([&restHandler](const crow::request& req, crow::response& res, uint64_t id) {
                restHandler.read_patient_basic_information(std::ref(req), std::ref(res), id);
            });

        CROW_ROUTE(app, "/api_v1/update_patient_basic_information/<int>")
            .methods("POST"_method)([&restHandler](const crow::request& req, crow::response& res, uint64_t id) {
                restHandler.update_patient_basic_information(std::ref(req), std::ref(res), id);
            });

        CROW_ROUTE(app, "/api_v1/delete_patient_basic_information/<int>")
            .methods("GET"_method)([&restHandler](const crow::request& req, crow::response& res, uint64_t id) {
                restHandler.delete_patient_basic_information(std::ref(req), std::ref(res), id);
            });

        // Start the server on port 8080
        std::cout << fmt::format("database server is started on port {}.\n", PORT);
        app.loglevel(crow::LogLevel::CRITICAL)
            .use_compression(crow::compression::algorithm::GZIP)
            .port(PORT)
            .concurrency(ncpus * 4)
            .bindaddr("0.0.0.0")
            .server_name("ProjectValhalla")
            //.multithreaded()
            .run();
    } catch (const std::exception& e) {
        std::cerr << "Exception caught in main: " << e.what() << std::endl;
        return 1; // Exit with error code
    }

    return 0;
}