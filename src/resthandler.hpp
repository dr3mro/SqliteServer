#include "databasehandler.hpp"
#include "threadpool.hpp"
#include <crow.h>

class RestHandler {
public:
    RestHandler(DatabaseHandler& dbHandler, ThreadPool& threadPool);

    // CRUD
    void create_patient_basic_information(const crow::request& req, crow::response& res);
    void read_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id);
    void update_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id);
    void delete_patient_basic_information(const crow::request& req, crow::response& res, uint64_t id);

private:
    DatabaseHandler& dbHandler;
    ThreadPool& threadPool;
};
