#include "usercontrollerimpl.hpp"
UserController::UserController(DatabaseController& dbController, RestHelper& rHelper)
    : pImpl(std::make_unique<Impl>(dbController, rHelper))
{
}

UserController::UserController()
{
}

UserController::~UserController()
{
}

void UserController::register_user(const crow::request& req, crow::response& res)
{
    json response_json;
    // Check JSON validity
    if (!pImpl->is_request_data_valid(req, res, response_json)) {
        return;
    }

    // Parse the JSON and extract the data
    try {
        // Get the JSON from request body
        auto userdata_json = json::parse(req.body);

        Impl::UserRegistrationData userRegistrationData;

        // Extract and validate user registration data
        if (!pImpl->extract_and_sanity_check_user_registration_data(userRegistrationData, userdata_json, response_json, res)) {
            return;
        }

        // Construct SQL query using {fmt} for parameterized query
        std::string query = fmt::format(
            "INSERT INTO users (username, password_hash, role, user_data) VALUES ('{}','{}','{}','{}')",
            userRegistrationData.username, userRegistrationData.password_hash, userRegistrationData.role, userRegistrationData.user_data);

        // Execute the query using DatabaseController
        json query_results_json = pImpl->dbController.executeQuery(query);

        pImpl->rHelper.evaluate_response(response_json, query_results_json);
        pImpl->rHelper.finish_response(res, 200, response_json);

    } catch (const std::exception& e) {
        // Handle exception (log, etc.)
        pImpl->respond_with_error(res, response_json, "Failure", fmt::format("Failed: {}", e.what()), -2, 500);
    }
}
