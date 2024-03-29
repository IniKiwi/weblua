#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <thread>
#include <string>
#include <map>
#include <unistd.h>
#include <sqlite3.h>
#include "route.h"

#include <luajit-2.1/lua.hpp>

#pragma once
#define WEBLUA_MAX_CLIENTS 2048

typedef struct{
    int sock;
    sockaddr_in addr;
    socklen_t len;
}client_t;

class server{
    private:
    static sockaddr_in server_addr;
    static int server_sock;
    static lua_State *server_lua_state;
    static char running;

    static void run_request(client_t _client);

    public:

    static int init(unsigned short port, std::string path);
    static int init_lua(std::string path);
    static int run_lua();
    static int main_loop();
    static void server_cli();
    static void log(std::string _log);
    static void show_error(std::string error, std::string reason);

    static int add_route(const char* _path, const char* _file);
};

