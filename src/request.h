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
#include <mutex>

#define WEBLUA_CHUNKED_PACKET_SIZE 65536
#define WEBLUA_MAX_FILE_SIZE 1099511627776

#pragma once
class request{
    private:
    std::string mimetype;
    std::string path;
    std::vector<uint8_t> data;
    std::string static_file;
    bool use_static_file;
    bool ended;
    size_t data_size;
    std::string status;
    std::string type;
    std::string client_request;
    client_t client;
    uint32_t id;
    static std::map<uint32_t,request*> requests;
    static uint32_t request_counter;
    static std::mutex mutex;

    public:
    request(lua_State* state, client_t _client);
    ~request();

    static request* get(uint32_t _id);
    static bool exists(uint32_t _id);
    static void rest_request_counter();

    void callback(lua_State* state);

    void send();
    void http_redirect(std::string _loc);

    void log(std::string str);

    void set_data(std::string _data);
    int load_file(std::string _file);
    void set_mimetype(std::string _mimetype);
    void set_status(std::string _status);
    void set_static_file(std::string _file);
    void set_use_static_file(bool _use);

    std::string get_data();
    std::string get_type();
    std::string get_mimetype();
    std::string get_status();
    client_t get_client();

    std::string get_form_field(std::string _field);
    std::string get_http_arg(std::string _arg);



};