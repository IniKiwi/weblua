#include <luajit-2.1/lua.hpp>

#pragma once

int l_add(lua_State* state);

int l_log(lua_State* state);

int l_get_ip(lua_State* state);

int l_set_mimetype(lua_State* state);

int l_set_data(lua_State* state);

int l_set_status(lua_State* state);