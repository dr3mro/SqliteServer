#include "databasehandler.hpp"

DatabaseHandler::DatabaseHandler(DatabaseConnectionPool& dbConnPool)
    : databaseConnectionPool(dbConnPool)
{
}

json DatabaseHandler::executeQuery(const std::string& query)
{
    auto db = databaseConnectionPool.get_connection();

    auto results = db->executeQuery(query);

    databaseConnectionPool.return_connection(db);

    return results;
}

void DatabaseHandler::executeNonQuery(const std::string& query)
{
    auto db = databaseConnectionPool.get_connection();

    db->executeNonQuery(query);

    databaseConnectionPool.return_connection(db);
}
