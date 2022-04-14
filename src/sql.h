#include <sqlite3.h>
#include <string>
#include <luajit-2.1/lua.hpp>
#pragma once

typedef struct{
    int type;
    std::string data_text;
    std::string key;
    double data_float;
}sql_storage_t;

class sql{
    private:
    static sqlite3* DB;
    sql_storage_t return_data;
    static char* message_error;
    static int callback(void* data, int argc, char** argv, char** azColName);
    public:
    static void init();
    static void close();
    static int set(sql_storage_t value);
    static sql_storage_t get(std::string key);
    static int l_create_table(lua_State* state);
    static int l_insert(lua_State* state);
    static int l_get(lua_State* state);
    static int l_update(lua_State* state);
    static int l_delete(lua_State* state);
    
};