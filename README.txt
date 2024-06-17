curl -H "Accept: application/json" -H "Content-type: application/json" -X POST -d '{"id":1,"value":"Mohamed Ali"}' http://localhost:18080/post


curl http://localhost:18080/get/104


Build the Docker image:

    docker build -t sqlserver .

Run the Docker container:

    docker run sqlserver

Get docker ID
    docker ps

Get docker IPAddress
    docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' e584361f0545
