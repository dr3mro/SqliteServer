#pragma once

#include <jwt-cpp/jwt.h>

class Tokenizer {
public:
    Tokenizer() = default;
    ~Tokenizer() = default;
    std::string generate_token(const std::string& username);
    bool token_validator(const std::string& token, const std::string& username);

private:
};