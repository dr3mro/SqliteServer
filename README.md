# 🚀 PROJECT VALHALLA

 - Before testing make sure you clean all the old containers.
 - cd into $PROJECT_DIR/docker and run those commands
```
docker compose down --rmi all --volumes
docker compose down --remove-orphans --volumes
docker compose up --build
```

### 🧪 Verify the server is running

```
curl http://172.20.0.2:8080/v1/hello
```
- if you get this reply `Welcome to ASGARD.%` that means the server is up and running
### 🚦 Resource not found
- if for any reason the server did not find the requested end point it will display this message

```
NOT FOUND, Are you lost?%
```


### 🎉 Create a new User
```
curl -X POST -H "Content-Type: application/json" -d @user.json http://172.20.0.2:8080/v1/user -i
```
- do a POST request on `/v1/user` with a body contains a JSON with following data
```
{
  "payload": {
    "fullname": "Amr Nasr",
    "username": "amr_nasr",
    "password": "123Mm@p0",
    "role": 0,
    "user_data": {
      "contact": {
        "phone": "+201299999999",
        "email": "amr@mail.com"
      },
      "address": {
        "city": "Damietta",
        "street": "portsaid street"
      },
      "dob": "1990-10-10",
      "gender": "male",
      "married": true,
      "job": {
        "position": "Doctor",
        "speciality": "Cardiology"
      }
    }
  },
  "sha256sum": "8657efb831ff66d61a6f26106a7acbe3f5a3bbc021b97fe1f1e4f69f21e20ec6",
  "api_key": "i7dEYwW9yeuwyuyw2qiqsajabgiqeifeghgjdafyuwti2g3289w2SDHWIHAhahaEAGEaGeaueaAegAgeaeG"
}
```
- a sucessful registeration will yield this result in json format.
```
HTTP/1.1 200 OK
Content-Length: 133
Server: ProjectValhalla
Date: Sat, 29 Jun 2024 13:38:55 GMT
Connection: Keep-Alive

{
    "payload": [
        {
            "affected rows": 1
        }
    ],
    "status id": 0,
    "status message": "success"
}%

```
- a failed regieration , for example user exists yields this results.
```
HTTP/1.1 400 Bad Request
Content-Length: 130
Server: ProjectValhalla
Date: Sat, 29 Jun 2024 13:36:13 GMT
Connection: Keep-Alive

{
    "payload": "User already exists",
    "status id": -1,
    "status message": "Failed to create a new user, user exists"
}%
```
- the "payload" value is verified on the server against SHA256SUM that should be generated and added to the JSON in sha256sum key.
- in the next updates there would be an `api_key` for creating users and should be supplied with every registeration but for now its not being used, currently "payload" "sha256sum" are the only needed.
- in the "payload" the username should only composed of lower case characters and numbers and not start with number and does not contains white spaces.
- the password should contains upper and lower case characters, symbols, numbers and at lest 8 characters long.
- the email should be in a valid format user@domain.ext
- the role value is for now '0' as the role implementation is WIP.

### 🚪 Login
```
 curl -X GET -H "Content-Type: application/json" -d @login.json http://172.20.0.2:8080/v1/user -i
```
- do a GET request on `/v1/user` with a body contains a JSON with following data
```
{
    "username" : "amr_nasr",
    "password" : "123Mm@p0"
}
```
- the username should be in lowecase and/or numbers and never contains spaces or symbols.
- the password should be uppercase and lowercase and characters and symbols.
- A successful login looks like this
```
HTTP/1.1 200 OK
Content-Length: 285
Server: ProjectValhalla
Date: Sat, 29 Jun 2024 21:44:00 GMT
Connection: Keep-Alive

{
    "payload": {
        "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MTk2OTgzNDAsImlhdCI6MTcxOTY5NzQ0MCwiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwic3ViIjoiYW1yX25hc3IifQ.2TOQNIIk-0gDxCSpNpHxVZOeN1u503viTHt8ezbcYDI"
    },
    "status id": 0,
    "status message": "success"
}%
```
- The returned token is valid only for 60 minutes and should be attached to every successding request and after it is expired any request will fail until the user relogin and get a new token.
- a failed login looks like this
``
HTTP/1.1 400 Bad Request
Content-Length: 128
Server: ProjectValhalla
Date: Sat, 29 Jun 2024 21:46:18 GMT
Connection: Keep-Alive

{
    "payload": "User 'amr_nasrm' not found or wrong password",
    "status id": -1,
    "status message": "Login Failure"
}%
``


### 👨🏻‍🦳 Add a patient
```
curl -X POST -H "Content-Type: application/json" -d @patient.json http://172.20.0.2:8080/v1/patient -i
```
- In order to add a new user do a POST request in `/v1/patient` with a body contains JSON like this.
```
{
  "payload": {
    "basic_data": {
      "id": 0,
      "firstname": "John",
      "lastname": "Doe",
      "date_of_birth": "1990-01-01",
      "gender": "Male",
      "place_of_birth": "New York",
      "address": "123 Main St, Anytown, USA",
      "occupation": "Engineer",
      "contact": [
        {
          "email": "john.doe@example.com"
        },
        {
          "phone": "+1987654321"
        }
      ]
    },
    "health_data": {
    },
    "appointments_data": {
    }
  },
  "sha256sum": "81eaf148afea6c1c6577e518f6c5e9987a128b0e451670be9f3c171b91205b17",
  "username": "amr_nasr",
  "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MTk2NjI2MjksImlhdCI6MTcxOTY2MTcyOSwiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwic3ViIjoiYW1yX25hc3IifQ.sDa6GIw-cve507uEth6tBfQ5OGfuAfxIl7P3JuSfS8c"
}
```
- The payload should contain 3 item:
  * "basic_data"
  * "appointments_data"
  * "health_data"
- The basic_data should contains {"id" = 0}.
- Providing empty value '{}' will clear the corresponding field in the database.
- Not providing a key for example "health_data" has no effect as only the provided data is processed.
- The payload should be verified with sha256sum and the hash should be provided in the JSON as "sha256sum" value.
- The username should exists and be valid.
- The access token should be valid.
- A successful request will return the user_id and it looks like this.
```
HTTP/1.1 200 OK
Content-Length: 869
Server: ProjectValhalla
Date: Sat, 29 Jun 2024 21:49:37 GMT
Connection: Keep-Alive

{
    "payload": [
        {
            "affected rows": 1
        },
        {
            "appointments_data": {},
            "basic_data": {
                "address": "123 Main St, Anytown, USA",
                "contact": [
                    {
                        "email": "john.doe@example.com"
                    },
                    {
                        "phone": "+1987654321"
                    }
                ],
                "date_of_birth": "1990-01-01",
                "firstname": "John",
                "gender": "Male",
                "id": 100034,
                "lastname": "Doe",
                "occupation": "Engineer",
                "place_of_birth": "New York"
            },
            "health_data": {},
            "id": 100034
        }
    ],
    "status id": 0,
    "status message": "success"
}%

```
- A failed request for example if the token becomes expired would look like this.
```
HTTP/1.1 400 Bad Request
Content-Length: 133
Server: ProjectValhalla
Date: Sat, 29 Jun 2024 13:49:52 GMT
Connection: Keep-Alive

{
    "payload": "authentication token invalidated",
    "status id": -1,
    "status message": "failed to create a new patient"
}%
```

### 🤓 Get a patient
```
curl -X GET -H "Content-Type: application/json" -d @get_patient.json http://172.20.0.2:8080/v1/patient -i
```
- do a GET request on `/v1/patient` with a body contains a JSON with following data
```
{
  "id": 100015,
  "schema":[
    "basic_data",
    "health_data",
    "appointments_data"
  ],
  "username": "amr_nasr",
  "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MTk2Njk4OTksImlhdCI6MTcxOTY2ODk5OSwiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwic3ViIjoiYW1yX25hc3IifQ.Ct9z0nLxdTCI1z3BclgjFQ0XZrBla3vwj_g30czZeGo"
}
```
- A successful request looks like this:
```
HTTP/1.1 200 OK
Content-Length: 784
Server: ProjectValhalla
Date: Sat, 29 Jun 2024 13:53:05 GMT
Connection: Keep-Alive

{
    "payload": [
        {
            "appointments_data": {},
            "basic_data": {
                "address": "123 Main St, Anytown, USA",
                "contact": [
                    {
                        "email": "john.doe@example.com"
                    },
                    {
                        "phone": "+1987654321"
                    }
                ],
                "date_of_birth": "1990-01-01",
                "firstname": "John",
                "gender": "Male",
                "id": 0,
                "lastname": "Doe",
                "occupation": "Engineer",
                "place_of_birth": "New York"
            },
            "health_data": {}
        }
    ],
    "status id": 0,
    "status message": "success"
}%
```
- A failed request due to for example expired access token looks something like this:
```
HTTP/1.1 400 Bad Request
Content-Length: 117
Server: ProjectValhalla
Date: Sat, 29 Jun 2024 13:52:36 GMT
Connection: Keep-Alive

{
    "payload": "token is invalidated",
    "status id": -1,
    "status message": "failed to retrieve patient"
}%
```
- the "schems" is an array of items you want to retrieve
- you can request 1 or more or even 0 but I did not test this case yet.
- the access token should be valid.
- the username should be valid.
- the 'id' is the patient_id and should exists

### 👍🏻 Update a patient
```
curl -X PATCH -H "Content-Type: application/json" -d @update_patient.json http://172.20.0.2:8080/v1/patient -i
```
- In order to update a patient do a PATCH request in `/v1/patient` with a body contains JSON like this.
```
{
  "payload": {
    "basic_data": {
      "id": 100001,
      "firstname": "Mark",
      "lastname": "Johnson",
      "date_of_birth": "1990-01-01",
      "gender": "Male",
      "place_of_birth": "Philadilphia",
      "address": "123 Main St, Anytown, USA",
      "occupation": "Nurse",
      "contact": [
        {
          "email": "mark@example.com"
        },
        {
          "phone": "+1987654321"
        }
      ]
    },
    "health_data": {
    },
    "appointments_data": {
    }
  },
  "sha256sum": "a57ba46b2f25823cbdbb69a839dd9db9cee7c6fb303302498a7e0516a3431315",
  "username": "amr_nasr",
  "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjIzNDkxMzUsImlhdCI6MTcxOTc1NzEzNSwiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwic3ViIjoiYW1yX25hc3IifQ.nMye8-rcyOIsbGqF4iNu0eXj-lxJ83puSh7hIdbYvHA"
}
```
- a successful delete reply with a json like this
```
HTTP/1.1 200 OK
Content-Length: 133
Server: ProjectValhalla
Date: Sun, 30 Jun 2024 19:47:04 GMT
Connection: Keep-Alive

{
    "payload": [
        {
            "affected rows": 1
        }
    ],
    "status id": 0,
    "status message": "success"
}%
```
- a failed request might be for example due to patient not found would look like this
```
HTTP/1.1 200 OK
Content-Length: 134
Server: ProjectValhalla
Date: Sun, 30 Jun 2024 19:52:33 GMT
Connection: Keep-Alive

{
    "payload": [
        {
            "affected rows": 0
        }
    ],
    "status id": -1,
    "status message": "failure"
}%
```
- Notice that it returns 200 OK but affected records 0 that means every step was successful except the query to the database.

### ❌ Delete a patient
  ```
  curl -X DELETE -H "Content-Type: application/json" -d @del_patient.json http://172.20.0.2:8080/v1/patient -i
  ```
  - In order to delete a patient do a DELETE request in `/v1/patient` with a body contains JSON like this.
  ```
  {
  "payload": {
    "basic_data": {
      "id": 100032,
      "firstname": "John",
      "lastname": "Doe",
      "date_of_birth": "1990-01-01",
      "gender": "Male"
    }
  },
  "sha256sum": "eb0ba7887fec5a0e369ae9cb56155ec9b0e13935af64a1af6adae31777e83951",
  "username": "amr_nasr",
  "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MTk3NTE0OTksImlhdCI6MTcxOTc1MDU5OSwiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwic3ViIjoiYW1yX25hc3IifQ.7i5QUrKpSX1sMlN6KVQZfiPzpT0CCHa7Uk2Gsy3J79w"
}
```
- data should be correct as any wrong info will invalidate the request.
- sha256sum is needed to prevent malicious mass deletion of data.
- a valid token must be provided.
- a successful delete reply with a json like this
```
HTTP/1.1 200 OK
Content-Length: 133
Server: ProjectValhalla
Date: Sun, 30 Jun 2024 12:33:40 GMT
Connection: Keep-Alive

{
    "payload": [
        {
            "affected rows": 1
        }
    ],
    "status id": 0,
    "status message": "success"
}%
```
- a failed delete request will reply with
```
HTTP/1.1 200 OK
Content-Length: 134
Server: ProjectValhalla
Date: Sun, 30 Jun 2024 12:34:23 GMT
Connection: Keep-Alive

{
    "payload": [
        {
            "affected rows": 0
        }
    ],
    "status id": -1,
    "status message": "failure"
}%
```
- notice that the http return code is `200` indicating it did reach the database but found no identical entry. it the data cannot get to the database due any other error the return code would be `400` like for example token invalidation or expiration.

```
HTTP/1.1 400 Bad Request
Content-Length: 134
Server: ProjectValhalla
Date: Sun, 30 Jun 2024 12:29:44 GMT
Connection: Keep-Alive

{
    "payload": "authentication token invalid or expired",
    "status id": -1,
    "status message": "failed to delete patient"
}%
```


### 🔎  Search
```
curl -X SEARCH -H "Content-Type: application/json" -d @search.json http://172.20.0.2:8080/v1/patient -i
```
 - In order to search for a patient do a SEARCH request in `/v1/patient` with a body contains JSON like this.

```
{
    "keyword" : "John",
    "limit" : 5,
    "offset" : 0,
    "order_by" : "id",
    "direction" : 0,
    "username" : "amr_nasr",
    "token" : "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjI0MjcwMDAsImlhdCI6MTcxOTgzNTAwMCwiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwic3ViIjoiYW1yX25hc3IifQ.i3IOtMR1tKpP86uMsnmCoU1227PxYnFB0z7aXXPyZts"
}
```
 - explanation:

    * keyword: the text to search for in basic data
    * limit: maximum number of returned results
    * offset : start with 0 and keep it to send it in the next request to get the next patch
    * order_by : name of json key to sort by
    * direction : ascending = 0 , descending = 1
    * user_name : of the user that is requesting the search
    * token : the valid token

- a reply would look like this
```
HTTP/1.1 200 OK
Content-Length: 2034
Server: ProjectValhalla
Date: Mon, 01 Jul 2024 13:23:47 GMT
Connection: Keep-Alive

{
    "more": true,
    "offset": 3,
    "payload": [
        {
            "basic_data": {
                "address": "123 Main St, Anytown, USA",
                "contact": [
                    {
                        "email": "john.doe@example.com"
                    },
                    {
                        "phone": "+1987654321"
                    }
                ],
                "date_of_birth": "1990-01-01",
                "firstname": "John",
                "gender": "Male",
                "id": 100009,
                "lastname": "Doe",
                "occupation": "Engineer",
                "place_of_birth": "New York"
            }
        },
        {
            "basic_data": {
                "address": "123 Main St, Anytown, USA",
                "contact": [
                    {
                        "email": "john.doe@example.com"
                    },
                    {
                        "phone": "+1987654321"
                    }
                ],
                "date_of_birth": "1990-01-01",
                "firstname": "John",
                "gender": "Male",
                "id": 100008,
                "lastname": "Doe",
                "occupation": "Engineer",
                "place_of_birth": "New York"
            }
        },
        {
            "basic_data": {
                "address": "123 Main St, Anytown, USA",
                "contact": [
                    {
                        "email": "john.doe@example.com"
                    },
                    {
                        "phone": "+1987654321"
                    }
                ],
                "date_of_birth": "1990-01-01",
                "firstname": "John",
                "gender": "Male",
                "id": 100007,
                "lastname": "Doe",
                "occupation": "Engineer",
                "place_of_birth": "New York"
            }
        }
    ],
    "status id": 0,
    "status message": "success"
}%
```

- check the `more` key to see if there is any more results
- keep the `offset` to use it with the next request.

🥳🥳🥳🥳🎉🎉🎉🎉

