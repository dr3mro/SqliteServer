cmake_minimum_required(VERSION 3.15)
project(sqlserver)

set(CMAKE_CXX_STANDARD 17)

include(CPM.cmake)

CPMAddPackage("gh:chriskohlhoff/asio#asio-1-30-2")
CPMAddPackage("gh:CrowCpp/crow#v1.2.0")
CPMAddPackage("gh:SRombauts/SQLiteCpp#3.3.1")

add_executable(sqlserver sqlserver.cpp connectionpool.cpp threadpool.cpp)

target_include_directories(sqlserver PRIVATE "${crow_SOURCE_DIR}/include" "${asio_SOURCE_DIR}/asio/include")

target_link_libraries(sqlserver PRIVATE SQLiteCpp )




