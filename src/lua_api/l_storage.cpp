#include <lua_api/l_storage.h>
#include <sql.h>

int l_set(lua_State* state){
    int args = lua_gettop(state);
    if(args == 2){
        sql_storage_t v;
        v.key = std::string(lua_tostring(state,1));
        if(lua_type(state, 2) == LUA_TSTRING){
            v.type = SQLITE_TEXT;
            v.data_text = std::string(lua_tostring(state,2));
        } else if(lua_type(state, 2) == LUA_TNUMBER) {
            v.type = SQLITE_FLOAT;
            v.data_float = lua_tonumber(state,2);
        } else {
            return 0;
        }
        sql::set(v);
    }
    return 0;
}

int l_get(lua_State* state){
    sql_storage_t data = sql::get(lua_tostring(state,1));
    if(data.type == SQLITE_TEXT){
        lua_pushstring(state, data.data_text.c_str());
        lua_pushnumber(state, data.type);
        return 2;
    }
    if(data.type == SQLITE_TEXT){
        lua_pushnumber(state, data.data_float);
        lua_pushnumber(state, data.type);
        return 2;
    }
    lua_pushnil(state);
    lua_pushnumber(state, 0);
    return 2;
}