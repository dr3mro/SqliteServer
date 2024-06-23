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

        CROW_ROUTE(app, "/api_v1/create_patient_basic_information")
            .methods("POST"_method)([&restHandler](const crow::request& req, crow::response& res) {
                restHandler.create_patient_basic_information(req, res);
            });
        // GET route example: /get/<int>
        CROW_ROUTE(app, "/api_v1/read_patient_basic_information/<int>")
            .methods("GET"_method)([&restHandler](const crow::request& req, crow::response& res, uint64_t id) {
                restHandler.read_patient_basic_information(req, res, id);
            });

        CROW_ROUTE(app, "/api_v1/update_patient_basic_information/<int>")
            .methods("POST"_method)([&restHandler](const crow::request& req, crow::response& res, uint64_t id) {
                restHandler.update_patient_basic_information(req, res, id);
            });

        CROW_ROUTE(app, "/api_v1/delete_patient_basic_information/<int>")
            .methods("GET"_method)([&restHandler](const crow::request& req, crow::response& res, uint64_t id) {
                restHandler.delete_patient_basic_information(req, res, id);
            });

        // Start the server on port 8080
        // app.loglevel(crow::LogLevel::Critical);
        app.use_compression(crow::compression::algorithm::GZIP).port(8080).multithreaded().run();
    } catch (const std::exception& e) {
        std::cerr << "Exception caught in main: " << e.what() << std::endl;
        return 1; // Exit with error code
    }

    return 0;
}