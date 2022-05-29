cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


project(WebLua)

set(
    SRCS
    src/main.cpp
    src/server.cpp
    src/lua_api/server.cpp
    src/lua_api/util.cpp
    src/route.cpp
    src/request.cpp
    src/sql.cpp
    src/lua_api/l_storage.cpp
)

set(
    HEADERS

)

add_executable(weblua ${SRCS} ${HEADERS})
include_directories(src)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package (SQLite3)
target_link_libraries(weblua PRIVATE Threads::Threads luajit-5.1 sqlite3 readline)

install(TARGETS weblua DESTINATION bin)
set(CMAKE_BUILD_TYPE Debug)