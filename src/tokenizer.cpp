#include "tokenizer.hpp"

#define ISSUER "ProjectValhalla"
#define TYPE "JWS"
#define VALIDITY 43200
#define SECRET "01234567890123456789012345678901"
// Function to generate JWT token
std::string Tokenizer::generate_token(const std::string& username)
{
    try {
        // Create JWT token with payload
        auto token = jwt::create()
                         .set_issuer(ISSUER)
                         .set_type(TYPE)
                         .set_subject(username)
                         .set_issued_at(std::chrono::system_clock::now())
                         .set_expires_at(std::chrono::system_clock::now() + std::chrono::minutes { VALIDITY })
                         .sign(jwt::algorithm::hs256 { SECRET });

        return token;
    } catch (const std::exception& e) {
        std::cerr << "Error generating token: " << e.what() << std::endl;
        return "";
    }
}

bool Tokenizer::token_validator(const std::string& token, const std::string& username)
{
    try {
        auto verifier = jwt::verify()
                            .with_issuer(ISSUER)
                            .with_type(TYPE)
                            .allow_algorithm(jwt::algorithm::hs256 { SECRET });

        auto decoded_token = jwt::decode(token);

        verifier.verify(decoded_token);

        // Check expiration

        auto exp_time = decoded_token.get_payload_claim("exp").as_date().time_since_epoch();
        auto subject = decoded_token.get_payload_claim("sub").as_string();
        auto issuer = decoded_token.get_payload_claim("iss").as_string();
        auto now = std::chrono::system_clock::now().time_since_epoch();

        if (now < exp_time && subject == username && issuer == ISSUER)
            return true;

    } catch (const std::exception& e) {
        std::cerr << "Error Validating token: " << e.what() << std::endl;
    }
    return false;
}