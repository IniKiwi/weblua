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
        request::get(lua_tointeger(state,1))->log(lua_tostring(state,2));
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
        request::get(lua_tointeger(state,1))->set_use_static_file(false);
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

int l_load_file(lua_State* state){
    int args = lua_gettop(state);
    if(args == 2){
        request::get(lua_tointeger(state,1))->set_static_file(lua_tostring(state,2));
        request::get(lua_tointeger(state,1))->set_use_static_file(true);
    }
    return 0;
}

int l_isPOST(lua_State* state){
    int args = lua_gettop(state);
    if(args == 1){
        if(request::get(lua_tointeger(state,1))->get_type() == "POST"){
            lua_pushboolean(state, true);
            return 1;
        }
    } 
    lua_pushboolean(state, false);
    return 1;
}

int l_get_form_feild(lua_State* state){
    int args = lua_gettop(state);
    if(args == 2){
        lua_pushstring(state, request::get(lua_tointeger(state,1))->get_form_field(lua_tostring(state,2)).c_str());
        return 1;
    }
    lua_pushnil(state);
    return 1;
}

int l_http_redirect(lua_State* state){
    int args = lua_gettop(state);
    if(args == 2){
        request::get(lua_tointeger(state,1))->http_redirect(lua_tostring(state,2));
    }
    return 0;
}