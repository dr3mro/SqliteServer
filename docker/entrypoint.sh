#!/bin/bash
#set -e

# Clone the repository if it doesn't exist
if [ ! -d .git ]; then
    git clone https://github.com/dr3mro/ProjectValhalla.git .
fi

# Pull the latest changes
git pull

# Build the project
cmake -S . -GNinja --trace
cmake --build . -j 2

# Execute the server
exec ./server
