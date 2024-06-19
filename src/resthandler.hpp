#ifndef REST_HANDLER_H
#define REST_HANDLER_H

#include "databaseconnectionpool.hpp"
#include <crow.h>
// RestHandler class
class RestHandler {
public:
    RestHandler(DatabaseConnectionPool& pool);

    // Handler for GET requests
    void handle_get(const crow::request& req, crow::response& res);

    // Handler for POST requests
    void handle_post(const crow::request& req, crow::response& res);

private:
    DatabaseConnectionPool& database_connection_pool;
};

#endif // REST_HANDLER_H
