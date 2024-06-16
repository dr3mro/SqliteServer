
#include "connectionpool.hpp"
#include "threadpool.hpp"

#include <crow.h>

int main()
{
    crow::SimpleApp app;

    ThreadPool threadPool(16);
    ConnectionPool connPool("database.db", 4);

    auto conn1 = connPool.getConnection();

    SQLite::Statement query2(*conn1,
        "CREATE TABLE IF NOT EXISTS data ("
        "id INTEGER,"
        "value TEXT);");
    query2.executeStep();

    // SQLite::Statement query1(*conn1, "PRAGMA journal_mode=WAL");
    // query1.executeStep();

    connPool.returnConnection(conn1);

    CROW_ROUTE(app, "/get/<int>")
    ([&](int id) {
        auto conn = connPool.getConnection();
        try {
            auto response = threadPool.enqueue([&conn, id]() {
                                          SQLite::Statement query(*conn, "SELECT value FROM data WHERE id = ?");
                                          query.bind(1, id);
                                          if (query.executeStep()) {
                                              return crow::response(200, query.getColumn(0).getText());
                                          } else {
                                              return crow::response(404, "Not Found");
                                          }
                                      })
                                .get();
            connPool.returnConnection(conn);
            return response;
        } catch (...) {
            connPool.returnConnection(conn);
            throw;
        }
    });

    CROW_ROUTE(app, "/post")
        .methods(crow::HTTPMethod::POST)([&](const crow::request& req) {
            auto conn = connPool.getConnection();
            try {
                auto response = threadPool.enqueue([&conn, &req]() {
                                              try {

                                                  auto json = crow::json::load(req.body);
                                                  if (!json) {
                                                      return crow::response(400, "Invalid JSON");
                                                  }
                                                  int id = json["id"].i();
                                                  std::string value = json["value"].s();

                                                  SQLite::Statement query(*conn, "INSERT INTO data (id, value) VALUES (?, ?)");
                                                  query.bind(1, id);
                                                  query.bind(2, value);
                                                  query.exec();

                                                  return crow::response(200, "Inserted");
                                              } catch (const std::exception& e) {
                                                  return crow::response(500, e.what());
                                              }
                                          })
                                    .get();
                connPool.returnConnection(conn);
                return response;
            } catch (...) {
                connPool.returnConnection(conn);
                throw;
            }
        });

    app.port(18080).multithreaded().run();
}
