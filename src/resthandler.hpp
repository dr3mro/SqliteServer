#include "databasehandler.hpp"
#include "threadpool.hpp"
#include <crow.h>

class RestHandler {
public:
    RestHandler(DatabaseHandler& dbHandler, ThreadPool& threadPool);

    void handle_get(const crow::request& req, crow::response& res, int id);
    void handle_post(const crow::request& req, crow::response& res);

private:
    DatabaseHandler& dbHandler;
    ThreadPool& threadPool;
};
