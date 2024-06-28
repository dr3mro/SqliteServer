#include "api_v1_routes.hpp"
#include "databaseconnectionpool.hpp"
#include "databasecontroller.hpp"
#include "patientcontroller.hpp"
#include "resthelper.hpp"
#include "usercontroller.hpp"
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

        // Create Patient Controller
        PatientController patientController(dbController, restHelper);

        // Create User Controller
        UserController userController(dbController, restHelper);

        // Initialize Crow application
        crow::SimpleApp app;

        // create routes for api v1
        API_V1_Routes routes(app, userController, patientController);

        // Start the server on port %PORT%
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