#include <lua_api/server.h>
#include <server.h>
#include <string>
#include <iostream>
#include <request.h>

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
        std::cout << "new callback url: " << lua_tostring(state, 1) << " " << lua_tostring(state, 2) << "\n";
    } else if(args == 2 && lua_type(state, 1) == LUA_TSTRING && lua_type(state, 2) == LUA_TFUNCTION){
        route* a = new route(route_type::CUSTOM,lua_tostring(state, 1));
        a->set_lua_callback(luaL_ref(state, LUA_REGISTRYINDEX));
        std::cout << "new custom url: " << lua_tostring(state, 1) << "\n";
    }
    return 0;
}

int l_log(lua_State* state){
    int args = lua_gettop(state);
    if(args == 2){
        std::cout << std::to_string(lua_tointeger(state,1)) << "> " << lua_tostring(state,2) << "\n";
    }
    return 0;
}

int l_get_ip(lua_State* state){
    int args = lua_gettop(state);
    if(args == 1){
        client_t cli = request::get(lua_tointeger(state,1))->get_client();
        lua_pushstring(state,inet_ntoa(cli.addr.sin_addr));
    } else{
        lua_pushnil(state);
    }
    return 1;
}

int l_set_mimetype(lua_State* state){
    int args = lua_gettop(state);
    if(args == 2){
        request::get(lua_tointeger(state,1))->set_mimetype(lua_tostring(state,2));
    }
    return 0;
}


int l_set_data(lua_State* state){
    int args = lua_gettop(state);
    if(args == 2){
        request::get(lua_tointeger(state,1))->set_data(lua_tostring(state,2));
    }
    return 0;
}

int l_set_status(lua_State* state){
    int args = lua_gettop(state);
    if(args == 2){
        request::get(lua_tointeger(state,1))->set_status(lua_tostring(state,2));
    }
    return 0;
}

int l_set_data_file(lua_State* state){
    int args = lua_gettop(state);
    if(args == 2){
        int r = request::get(lua_tointeger(state,1))->set_data_file(lua_tostring(state,2));
        if(r==0){
            lua_pushboolean(state,true);
            return 1;
        }
        
    }
    lua_pushnil(state);
    return 1;
}