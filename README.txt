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


To set up a Docker container running PostgreSQL with a static IP address, you typically need to create a Docker network and then attach your PostgreSQL container to that network with a specified IP address. Here’s a step-by-step guide to achieve this:
Step 1: Create a Docker network

First, create a Docker network that will be used for your PostgreSQL container. This allows you to have control over IP address assignment.

bash

docker network create --subnet=192.168.100.0/24 postgres-network

Replace 192.168.100.0/24 with the subnet you want to use for your network.
Step 2: Run the PostgreSQL container

Now, run a PostgreSQL container and attach it to the network with a static IP address.

bash

docker run --name postgres-container --net postgres-network --ip 192.168.100.10 \
    -e POSTGRES_PASSWORD=mysecretpassword -d postgres

    --name postgres-container: Assigns a name to the container for easier management.
    --net postgres-network: Attaches the container to the postgres-network network.
    --ip 192.168.100.10: Specifies the static IP address you want to assign to the container.
    -e POSTGRES_PASSWORD=mysecretpassword: Sets the PostgreSQL database password. You can adjust other environment variables (POSTGRES_USER, etc.) as needed for your setup.
    -d postgres: Specifies the Docker image to use (in this case, the official PostgreSQL image).

Step 3: Verify connectivity

Once the container is running, you can verify connectivity by connecting to the PostgreSQL database from another container or from the host machine using the specified static IP address (192.168.100.10 in this example).

bash

psql -h 192.168.100.10 -U postgres

Notes:

    Networking: Make sure the chosen subnet (192.168.100.0/24 in the example) does not conflict with your existing network configuration.
    Static IP Assignment: Docker does not natively support assigning static IPs directly through container creation options, but this method effectively achieves the same result by creating a custom network and specifying the IP.
    Persistence: For persistent data, consider using Docker volumes to store PostgreSQL data outside the container.

By following these steps, you can set up a PostgreSQL container with a static IP address within Docker, providing predictable networking for your applications. Adjust the IP addresses and network settings according to your specific requirements and network configuration.
