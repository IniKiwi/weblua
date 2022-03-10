#include "server.h"
#include <lua_api/server.h>
#include <iostream>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <vector>
#include <fstream>

uint32_t server::requests = 0;
client_t server::client_tmp;
lua_State* server::server_lua_state;
std::map<uint32_t,client_t> server::clients;
std::map<std::string,std::string> server::URLs;
sockaddr_in server::server_addr;
int server::server_sock;

int server::init(unsigned short port){
    server::server_addr.sin_family = AF_INET; 
    server::server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    server::server_addr.sin_port = htons(port); 

    server::server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(bind(server::server_sock,(struct sockaddr *)&server::server_addr,sizeof(server::server_addr)) < 0) { 
        std::cerr << "bind failed!\nthe port "<<port<<" is free?\n";
        exit(EXIT_FAILURE); 
    }
    return 0;
}

void server::run_request(uint32_t _id){
    char buffer[50000] = {0}; 
    read(server::clients.at(_id).sock, buffer, 50000);
    std::string buff = buffer;
    //std::cout << buff;

    std::vector<std::string> words;
    std::istringstream stream(buff);
    std::copy(
        std::istream_iterator<std::string>( stream ),
        std::istream_iterator<std::string>(),
        std::back_inserter( words )
    );

    std::cout << words[0] << " " << words[1] << " " << words[2] << " -> ";

    if(server::URLs.find(words[1]) == server::URLs.end()){
        std::cout << "404\n";
        std::string buf = "HTTP/1.1 404 Not Found\nContent-Type: text/plain\nContent-Length: 4\n\n404!";
        write(server::clients.at(_id).sock, buf.c_str(), buf.length());
    } else {
        std::ifstream file;
        std::streampos fb, fe;

        file.open(server::URLs.at(words[1]), std::ios::binary);
        file.seekg(0, std::ios::end);
        int file_size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::string buf = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: "+std::to_string(file_size)+"\n\n" + std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        write(server::clients.at(_id).sock, buf.c_str(), buf.length());
        std::cout << "200\n";
    }
    close(server::clients.at(_id).sock);
}

int server::add_route(const char* _path, const char* _file){
    std::string path = _path;
    std::string file = _file;
    server::URLs.insert(std::pair<std::string,std::string>(path,file));
    std::cout << "new url: " << path << " " << file << "\n";
    return 0;
}

int server::main_loop(){
    listen(server_sock, 20);
    while(1){
        if((server::client_tmp.sock = accept(server::server_sock, (sockaddr*)&server::client_tmp.addr, (socklen_t*)&server::client_tmp.len))<0){
            std::cerr << "accept failed!\n";
        }
        server::clients.insert(std::pair<uint32_t,client_t>(server::requests, server::client_tmp));
        new std::thread(&server::run_request,server::requests);
        server::requests++;
    }
}

int server::init_lua(){
    server::server_lua_state = luaL_newstate();
    luaL_openlibs(server::server_lua_state);
    lua_register(server::server_lua_state, "weblua_add", l_add);
    int restlt2 = luaL_loadfile(server::server_lua_state, "/home/pierre/c-cpp-dev/weblua/testapp/test.lua");
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