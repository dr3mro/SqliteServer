#include "resthandler.hpp"

RestHandler::RestHandler(DatabaseHandler& dbHandler, ThreadPool& threadPool)
    : dbHandler(dbHandler)
    , threadPool(threadPool)
{
}

void RestHandler::handle_get(const crow::request& req, crow::response& res, int id)
{
    threadPool.enqueue([this, &res, id]() {
        std::string query = "SELECT * FROM data WHERE id = " + std::to_string(id);
        json result = dbHandler.executeQuery(query);

        if (result.empty()) {
            res.code = 404;
            res.write("Not Found");
        } else {
            res.write(result.dump(4)); // Pretty print JSON with 4 spaces indentation
        }
        res.end();
    });
}

void RestHandler::handle_post(const crow::request& req, crow::response& res)
{
    auto jsonData = json::parse(req.body);
    int id = jsonData["id"];
    std::string value = jsonData["value"];
    std::string query = "INSERT INTO data (id, value) VALUES (" + std::to_string(id) + ", '" + value + "')";

    threadPool.enqueue([this, &res, query]() {
        dbHandler.executeNonQuery(query);
        res.code = 200;
        res.write("Inserted successfully");
        res.end();
    });
}
