cmake_minimum_required(VERSION 3.15)
project(sqlserver)

set(CMAKE_CXX_STANDARD 17)

# Set the module path to include a custom directory
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )
include(CPM)

CPMAddPackage("gh:chriskohlhoff/asio#asio-1-30-2")
CPMAddPackage("gh:CrowCpp/crow#v1.2.0")
CPMAddPackage("gh:SRombauts/SQLiteCpp#3.3.1")

file (GLOB_RECURSE SRC "src/*.cpp" )

add_executable(sqlserver ${SRC})

target_include_directories(sqlserver PRIVATE "${crow_SOURCE_DIR}/include" "${asio_SOURCE_DIR}/asio/include")

target_link_libraries(sqlserver PRIVATE SQLiteCpp )




