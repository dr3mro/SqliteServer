#include "databasehandler.hpp"
#include <iostream>

DatabaseHandler::DatabaseHandler(DatabaseConnectionPool& dbConnPool)
    : databaseConnectionPool(dbConnPool)
{
}

json DatabaseHandler::executeQuery(const std::string& query)
{
    std::shared_ptr<Database> db = nullptr;
    try {
        db = databaseConnectionPool.get_connection();
        json results = db->executeQuery(query);
        databaseConnectionPool.return_connection(db);
        return results;
    } catch (const std::exception& e) {
        std::cerr << "Exception occurred during executeQuery: " << e.what() << std::endl;
        // Optionally handle or log the exception here
        throw; // Rethrow the exception to indicate failure
    } catch (...) {
        std::cerr << "Unknown exception occurred during executeQuery." << std::endl;
        // Optionally handle or log the exception here
        throw; // Rethrow the exception to indicate failure
    }
    return json(); // Return empty JSON object on failure (if no rethrow)
}

json DatabaseHandler::executeReadQuery(const std::string& query)
{
    std::shared_ptr<Database> db = nullptr;
    try {
        db = databaseConnectionPool.get_connection();
        json results = db->executeReadQuery(query);
        databaseConnectionPool.return_connection(db);
        return results;
    } catch (const std::exception& e) {
        std::cerr << "Exception occurred during executeQuery: " << e.what() << std::endl;
        // Optionally handle or log the exception here
        throw; // Rethrow the exception to indicate failure
    } catch (...) {
        std::cerr << "Unknown exception occurred during executeQuery." << std::endl;
        // Optionally handle or log the exception here
        throw; // Rethrow the exception to indicate failure
    }
    return json(); // Return empty JSON object on failure (if no rethrow)
}
