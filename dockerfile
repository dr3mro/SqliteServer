# Use an official Ubuntu as a parent image
FROM ubuntu:latest

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    git \
    cmake \
    gcc \
    g++ \
    ninja-build \
    python3 \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /app

# Clone the repository
RUN git clone https://github.com/dr3mro/SqliteServer.git .

# Configure the project with CMake
RUN cmake -S. -Bbuild -G Ninja

# Build the project
RUN cmake --build build

# Run the binary
CMD [ "python3", "print_ip.py", "&&", "./build/sqlserver"]
