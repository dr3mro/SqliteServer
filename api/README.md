# PROJECT VALHALLA
### user registeration
```
$ curl -X POST -H "Content-Type: application/json" -d @user.json http://localhost:8080/v1/register -i
```
- do a POST request on /v1/register with a body contains a JSON with following data
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
  "username": "admin",
  "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MTk2NjI2MjksImlhdCI6MTcxOTY2MTcyOSwiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwic3ViIjoiYW1yX25hc3IifQ.sDa6GIw-cve507uEth6tBfQ5OGfuAfxIl7P3JuSfS8c"
}
```
- the "payload" value is verified on the server against SHA256SUM that should be generated and added to the JSON in sha256sum key.
- in the next updates there would be a token for creating users and should be supplied with every registeration but for now its not being used, currently "payload" "sha256sum" are the only needed.
- in the "payload" the username should only composed of lower case characters and numbers and not start with number and does not contains white spaces.
- the password should contains upper and lower case characters, symbols, numbers and at lest 8 characters long.
- the email should be in a valid format user@domain.ext
- the role value is for now '0' as the role implementation is WIP.

### user login
```
 curl -X POST -H "Content-Type: application/json" -d @login.json http://localhost:8080/v1/login -i
```
- do a POST request on /v1/login with a body contains a JSON with following data
```
{
    "username" : "amr_nasr",
    "password" : "123Mm@p0"
}
```
- the username should be in lowecase and/or numbers and never contains spaces or symbols.
- the password should be uppercase and lowercase and characters and symbols.


### patient add

### patient get

### patient update
 - WIP
### patient delete
 - WIP
### patient search
 - WIP