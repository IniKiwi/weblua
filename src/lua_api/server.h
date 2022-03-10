#include <luajit-2.1/lua.hpp>

#pragma once

int l_add(lua_State* state);

const luaL_Reg webluafuncs[] = {
    {"add",l_add},
    {NULL,NULL}
};