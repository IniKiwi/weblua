#include <luajit-2.1/lua.hpp>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#pragma once

enum class route_type{
    PATH,
    PATH_CALLBACK,
    CUSTOM
};

class route{
    private:
    route_type type;
    std::string file;
    std::string mimetype;
    std::string path;
    int r_callback;
    static std::map<std::string,route*> routes;
    public:
    route(route_type _type, std::string _path);
    ~route();

    static route* get(std::string _path);
    static bool exists(std::string _path);

    void callback(lua_State* state, uint32_t _id);
    void detect_mimetype();

    void set_type(route_type _type);
    void set_file(std::string _file);
    void set_mimetype(std::string _mimetype);
    void set_route(std::string _path);
    void set_lua_callback(int _callback);

    route_type get_type();
    std::string get_mimetype();
    std::string get_route();
    std::string get_file();
    int get_lua_callback();
};