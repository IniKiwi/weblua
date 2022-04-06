#include "server.h"
#include "lua_api/util.h"
#include <lua_api/server.h>
#include <lua_api/l_storage.h>
#include <iostream>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <vector>
#include <chrono>
#include <fstream>
#include <request.h>
#include <csignal>
#include <sql.h>
#include <readline/readline.h>
#include <readline/history.h>

lua_State* server::server_lua_state;
sockaddr_in server::server_addr;
int server::server_sock;
char server::running;

int server::init(unsigned short port, std::string path){
    signal(SIGPIPE, SIG_IGN);
    server::running = 1;
    server::server_addr.sin_family = AF_INET; 
    server::server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    server::server_addr.sin_port = htons(port); 
    request::rest_request_counter();
    sql::init();

    server::server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(bind(server::server_sock,(struct sockaddr *)&server::server_addr,sizeof(server::server_addr)) < 0) { 
        std::cerr << "bind failed!\nthe port "<<port<<" is free?\n";
        exit(EXIT_FAILURE); 
    }

    init_lua(path);
    new std::thread(main_loop);
    server_cli();

    return 0;
}

void server::run_request(client_t _client){
    request* rq = new request(server::server_lua_state, _client);
    delete rq;
}

void server::server_cli(){
    while(1){
        char * line = readline("\r> ");
        if(!line) break;
        if(*line) add_history(line);
        std::string com = line;
        if(com == "exit"){
            running = false;
            close(server::server_sock);
            free(line);
            break;
        } else if (com == "help"){
            server::log("exit: exit weblua and stop the server");
        } else if(com == ""){

        }
        else {
            server::log("\e[31mno command named "+com);
        }
        free(line);
    }
}

void server::log(std::string _log){
    std::cout << "\r\e[39m> " << _log << "\e[39m\r\n> ";
}

int server::main_loop(){
    listen(server_sock, WEBLUA_MAX_CLIENTS);
    client_t client_tmp;
    while(server::running){
        if((client_tmp.sock = accept(server::server_sock, (sockaddr*)&client_tmp.addr, (socklen_t*)&client_tmp.len))<0){
            std::cerr << "accept failed!\n";
        } else {
            new std::thread(&server::run_request,client_tmp);
        }
        
    }
    close(server_sock);
    return 0;
}

int server::init_lua(std::string path){
    server::server_lua_state = luaL_newstate();
    luaL_openlibs(server::server_lua_state);
    register_c_function(server::server_lua_state,"weblua","add",l_add);
    register_c_function(server::server_lua_state,"weblua","log",l_log);
    register_c_function(server::server_lua_state,"weblua","get_ip",l_get_ip);
    register_c_function(server::server_lua_state,"weblua","set_mimetype",l_set_mimetype);
    register_c_function(server::server_lua_state,"weblua","set_status",l_set_status);
    register_c_function(server::server_lua_state,"weblua","set_data",l_set_data);
    register_c_function(server::server_lua_state,"weblua","load_file",l_load_file);
    register_c_function(server::server_lua_state,"weblua","isPOST",l_isPOST);
    register_c_function(server::server_lua_state,"weblua","get_form_feild",l_get_form_feild);
    register_c_function(server::server_lua_state,"weblua","http_redirect",l_http_redirect);
    register_c_function(server::server_lua_state,"weblua","render",l_render);
    register_c_function(server::server_lua_state,"storage","set",l_set);
    register_c_function(server::server_lua_state,"storage","get",l_get);
    lua_pushnumber(server::server_lua_state, SQLITE_TEXT);
    lua_setglobal(server::server_lua_state, "STRING");
    lua_pushnumber(server::server_lua_state, SQLITE_FLOAT);
    lua_setglobal(server::server_lua_state, "NUMBER");
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