curl -H "Accept: application/json" -H "Content-type: application/json" -X POST -d '{"id":1,"value":"Mohamed Ali"}' http://localhost:8080/post


curl http://localhost:8080/get/1


Build the Docker image:

    docker build -t sqlserver .

Run the Docker container:

    docker run sqlserver

Get docker ID
    docker ps

Get docker IPAddress
    docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' e584361f0545



    docker pull postgres
    docker volume create postgres_data
    docker run --name postgres_container -e POSTGRES_PASSWORD=000 -d -p 5432:5432 -v postgres_data:/var/lib/postgresql/data postgres
