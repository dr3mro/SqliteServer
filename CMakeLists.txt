cmake_minimum_required(VERSION 3.15)

set(Target "server")
project(${Target})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )
include(CPM)

CPMAddPackage("gh:nlohmann/json#v3.11.3")
CPMAddPackage("gh:fmtlib/fmt#10.2.1")
CPMAddPackage("gh:chriskohlhoff/asio#asio-1-30-2")
CPMAddPackage("gh:CrowCpp/crow#v1.2.0")
CPMAddPackage("gh:jtv/libpqxx#7.9.1")

file (GLOB_RECURSE SRC "src/*.cpp" )

add_executable(${Target} ${SRC})

target_include_directories(${Target}
                        PRIVATE
                        "${json_SOURCE_DIR}/include"
                        "${fmt_SOURCE_DIR}/include"
                        "${crow_SOURCE_DIR}/include"
                        "${asio_SOURCE_DIR}/asio/include"
                        "${libpqxx_SOURCE_DIR}/include"
                        )

target_link_libraries(${Target}  PRIVATE  fmt pqxx ${PostgreSQL_LIBRARIES})




