#include <luajit-2.1/lua.hpp>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <server.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#pragma once
class request{
    private:
    std::string mimetype;
    std::string path;
    std::string data;
    std::string status;
    std::string client_request;
    client_t client;
    uint32_t id;
    static std::map<uint32_t,request*> requests;
    static uint32_t request_counter;

    public:
    request(lua_State* state, client_t _client);
    ~request();

    static request* get(uint32_t _id);
    static bool exists(uint32_t _id);
    static void rest_request_counter();

    void callback(lua_State* state);

    void send();


};