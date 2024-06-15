cmake_minimum_required(VERSION 3.15)
project(RestfulAPI)

set(CMAKE_CXX_STANDARD 11)

# Include CPM.cmake
include(CPM.cmake)

# Add Crow dependency
CPMAddPackage(
    NAME Crow
    GIT_REPOSITORY https://github.com/CrowCpp/crow.git
    GIT_TAG master
)

# Add SQLiteCpp dependency
CPMAddPackage(
    NAME SQLiteCpp
    GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
    GIT_TAG master
)

# Include directories
include_directories(${Crow_SOURCE_DIR}/include ${SQLiteCpp_SOURCE_DIR}/include)

# Link directories
link_directories(${SQLiteCpp_SOURCE_DIR}/lib)

# Specify the source files
add_executable(sqlserver sqlserver.cpp connectionpool.cpp threadpool.cpp)

# Link against Crow and SQLiteCpp
target_link_libraries(sqlserver Crow::Crow SQLiteCpp sqlite3 pthread)
