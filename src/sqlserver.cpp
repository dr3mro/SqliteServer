#include "connectionpool.hpp"
#include "threadpool.hpp"
#include <chrono>
#include <crow.h>
#include <thread>

#include "database.hpp"

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

    Database pqdb("host = 172.17.0.2 dbname = postgres user = postgres password = 000");

    crow::SimpleApp app;

    ThreadPool threadPool(128);
    ConnectionPool connPool("database.db", 128);

    auto conn1 = connPool.getConnection();
    SQLite::Statement query2(*conn1,
        "CREATE TABLE IF NOT EXISTS data ("
        "id INTEGER,"
        "value TEXT);");
    executeWithRetry([&]() { query2.executeStep(); });
    connPool.returnConnection(conn1);

    CROW_ROUTE(app, "/get/<int>")
    ([&](int id) {
        auto conn = connPool.getConnection();
        try {
            auto response = threadPool.enqueue([&conn, id]() {
                                          return executeWithRetry([&]() {
                                              SQLite::Statement query(*conn, "SELECT value FROM data WHERE id = ?");
                                              query.bind(1, id);
                                              if (query.executeStep()) {
                                                  return crow::response(200, query.getColumn(0).getText());
                                              } else {
                                                  return crow::response(404, "Not Found");
                                              }
                                          });
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
                                              return executeWithRetry([&]() {
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
                                              });
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