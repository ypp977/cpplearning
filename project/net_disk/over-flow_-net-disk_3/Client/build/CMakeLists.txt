cmake_minimum_required(VERSION 3.0)
project(NetDisk_Client)


set(CMAKE_BUILD_TYPE Debug)

include_directories(${PROJECT_SOURCE_DIR}/../Include)

file(GLOB SOURCES ../src/*.c)

add_executable(${PROJECT_NAME} ${SOURCES})
link_directories(/usr/lib/x86_64-linux-gnu)
target_link_libraries(${PROJECT_NAME} pthread mysqlclient crypt)
