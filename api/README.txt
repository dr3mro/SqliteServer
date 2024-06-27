# register a user

send POST request to /api_v1/register with user1.json
curl -X POST -H "Content-Type: application/json" -d @user1.json http://localhost:8080/v1/register -i

if success :

{
    "response": [
        {
            "affected rows": 1
        }
    ],
    "status id": 0,
    "status message": "success"
}

if user exists:

{
    "response": "user already exists",
    "status id": -1,
    "status message": "failed to create a new user, user exists"
}%



====

