cmake_minimum_required(VERSION 3.14)

PROJECT(server)

set(SERVER_EXE server.cpp udpconn.cpp)	

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

add_executable(server ${SERVER_EXE})

target_include_directories(server PRIVATE ${argparse_SOURCE_DIR}/include)
target_include_directories(server PRIVATE ${json_SOURCE_DIR}/include)

target_link_libraries(server PRIVATE argparse nlohmann_json::nlohmann_json)