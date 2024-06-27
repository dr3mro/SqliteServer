#



# register a user

send POST request to
            /api_v1/register

with user1.json

            $ curl -X POST -H "Content-Type: application/json" -d @user1.json http://localhost:8080/v1/register -i

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

HTTP/1.1 400 Bad Request
Content-Length: 132
Server: ProjectValhalla
Date: Thu, 27 Jun 2024 17:08:34 GMT
Connection: Keep-Alive

{
    "response": "user already exists",
    "status id": -1,
    "status message": "failed to create a new user, user exists"
}%

if invalid json :

HTTP/1.1 400 Bad Request
Content-Length: 132
Server: ProjectValhalla
Date: Thu, 27 Jun 2024 17:08:34 GMT
Connection: Keep-Alive
{
    "response": "error parsing user data: Unexpected end of file at line 1 and column 490",
    "status id": -1,
    "status message": "failed to create a new user, invalid json"
}%

if weak password:

HTTP/1.1 400 Bad Request
Content-Length: 132
Server: ProjectValhalla
Date: Thu, 27 Jun 2024 17:08:34 GMT
Connection: Keep-Alive
{
    "response": "password in weak",
    "status id": -1,
    "status message": "failed to create a new user, invalid password"
}%

if invalid username:

HTTP/1.1 400 Bad Request
Content-Length: 132
Server: ProjectValhalla
Date: Thu, 27 Jun 2024 17:08:34 GMT
Connection: Keep-Alive
{
    "response": "username contains spaces",
    "status id": -1,
    "status message": "failed to create a new user, username contains spaces"
}%

if empty username or password:

HTTP/1.1 400 Bad Request
Content-Length: 132
Server: ProjectValhalla
Date: Thu, 27 Jun 2024 17:08:34 GMT
Connection: Keep-Alive
{
    "response": "empty username or password",
    "status id": -1,
    "status message": "failed to create a new user, invalid data"
}%

if invalid email:

HTTP/1.1 400 Bad Request
Content-Length: 132
Server: ProjectValhalla
Date: Thu, 27 Jun 2024 17:08:34 GMT
Connection: Keep-Alive
{
    "response": "invalid email format",
    "status id": -1,
    "status message": "failed to create a new user, invalid data"
}%

====

