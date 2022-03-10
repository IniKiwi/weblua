#include "../lua_api/server.h"
#include "../server.h"
#include <string>
#include <iostream>

int l_add(lua_State* state){
    int args = lua_gettop(state);
    if(lua_type(state, 2) == LUA_TSTRING){
        server::add_route(lua_tostring(state, 1),lua_tostring(state, 2));
    } else if(lua_type(state, 2) == LUA_TFUNCTION){
        
    }
    return 0;
}
