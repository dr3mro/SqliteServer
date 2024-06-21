#include "databasehandler.hpp"
#include "threadpool.hpp"
#include <crow.h>

class RestHandler {
public:
    RestHandler(DatabaseHandler& dbHandler, ThreadPool& threadPool);

    void handle_get_patient_basic_information(const crow::request& req, crow::response& res, int id);
    void handle_create_client_personal_history(const crow::request& req, crow::response& res);

private:
    DatabaseHandler& dbHandler;
    ThreadPool& threadPool;
};
