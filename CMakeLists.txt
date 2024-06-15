cmake_minimum_required(VERSION 3.15)
project(sqlserver)

set(CMAKE_CXX_STANDARD 17)

# Include CPM.cmake
include(CPM.cmake)

# Add ASIO dependency
CPMAddPackage(
    NAME Asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-30-2
)

# Add Crow dependency
CPMAddPackage(
    NAME Crow
    GIT_REPOSITORY https://github.com/CrowCpp/crow.git
    GIT_TAG v1.2.0
)

# Add SQLiteCpp dependency
CPMAddPackage(
    NAME SQLiteCpp
    GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
    GIT_TAG 3.3.1
)

# Specify the source files
add_executable(sqlserver sqlserver.cpp connectionpool.cpp threadpool.cpp)


# Link against Crow, SQLiteCpp, and other necessary libraries
target_link_libraries(sqlserver PRIVATE Crow SQLiteCpp pthread)
