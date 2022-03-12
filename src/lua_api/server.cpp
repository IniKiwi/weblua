#include "../lua_api/server.h"
#include "../server.h"
#include <string>
#include <iostream>

int l_add(lua_State* state){
    int args = lua_gettop(state);
    if(args == 2 && lua_type(state, 1) == LUA_TSTRING && lua_type(state, 2) == LUA_TSTRING){ //weblua.add(path,file)
        route* a = new route(route_type::PATH,lua_tostring(state, 1));
        a->set_file(lua_tostring(state, 2));
        std::cout << "new url: " << lua_tostring(state, 1) << " " << lua_tostring(state, 2) << "\n";
    } 
    else if(args == 3 && lua_type(state, 1) == LUA_TSTRING  //weblua.add(path,file, callback)
    && lua_type(state, 2) == LUA_TSTRING 
    && lua_type(state, 3) == LUA_TFUNCTION){ 
        route* a = new route(route_type::PATH_CALLBACK,lua_tostring(state, 1));
        a->set_file(lua_tostring(state, 2));
        a->set_lua_callback(luaL_ref(state, LUA_REGISTRYINDEX));
        std::cout << "new url, callback: " << lua_tostring(state, 1) << " " << lua_tostring(state, 2) << "\n";
    }
    return 0;
}
