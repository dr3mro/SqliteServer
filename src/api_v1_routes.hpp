#pragma once
#include "patientcontroller.hpp"
#include "usercontroller.hpp"
#include <crow.h>

class API_V1_Routes {

public:
    API_V1_Routes(crow::SimpleApp& app, UserController& userController, PatientController& patientController);

private:
    crow::SimpleApp& app;
    UserController& userController;
    PatientController& patientController;
};