cmake_minimum_required(VERSION 3.0)

project(WebLua)

set(
    SRCS
    src/main.cpp
    src/server.cpp
    src/lua_api/server.cpp
    src/lua_api/util.cpp
)

set(
    HEADERS

)

add_executable(weblua ${SRCS} ${HEADERS})
include_directories(src)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(weblua PRIVATE Threads::Threads luajit-5.1)