# Dockerfile.pq

FROM postgres:latest

# Copy the initialization script
COPY init.sql /docker-entrypoint-initdb.d/
