#include "server.h"
#include "lua_api/util.h"
#include <lua_api/server.h>
#include <iostream>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <request.h>

lua_State* server::server_lua_state;
sockaddr_in server::server_addr;
int server::server_sock;

int server::init(unsigned short port){
    server::server_addr.sin_family = AF_INET; 
    server::server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    server::server_addr.sin_port = htons(port); 
    request::rest_request_counter();

    server::server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(bind(server::server_sock,(struct sockaddr *)&server::server_addr,sizeof(server::server_addr)) < 0) { 
        std::cerr << "bind failed!\nthe port "<<port<<" is free?\n";
        exit(EXIT_FAILURE); 
    }
    return 0;
}

void server::run_request(client_t _client){
    request* rq = new request(server::server_lua_state, _client);
    rq->send();
    delete rq;
}

int server::main_loop(){
    listen(server_sock, 20);
    client_t client_tmp;
    while(1){
        if((client_tmp.sock = accept(server::server_sock, (sockaddr*)&client_tmp.addr, (socklen_t*)&client_tmp.len))<0){
            std::cerr << "accept failed!\n";
        }
        new std::thread(&server::run_request,client_tmp);
    }
}

int server::init_lua(std::string path){
    server::server_lua_state = luaL_newstate();
    luaL_openlibs(server::server_lua_state);
    register_c_function(server::server_lua_state,"weblua","add",l_add);
    register_c_function(server::server_lua_state,"weblua","log",l_log);
    int restlt2 = luaL_loadfile(server::server_lua_state, path.c_str());
    if(restlt2 != LUA_OK){
        std::cout << "lua loadfile error:\n";

    }
    std::cout << "lua register ended\n";

    int result = lua_pcall(server::server_lua_state, 0, LUA_MULTRET, 0);

    if(result != LUA_OK) {
        const char* message = lua_tostring(server::server_lua_state, -1);
        std::cout << "error at pcall:\n";
        puts(message);
        lua_pop(server::server_lua_state, 1);
        return 1;
    }
    std::cout << "lua script   ended\n";
    return 0;
}

int server::run_lua(){
    return 0;
}