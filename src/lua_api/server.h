#include <luajit-2.1/lua.hpp>
#include <string>

#pragma once

int l_add(lua_State* state);

int l_log(lua_State* state);

int l_get_ip(lua_State* state);

int l_set_mimetype(lua_State* state);

int l_set_data(lua_State* state);

int l_set_status(lua_State* state);

int l_set_data_file(lua_State* state);

int l_load_file(lua_State* state);

int l_isPOST(lua_State* state);

int l_get_form_feild(lua_State* state);

int l_http_redirect(lua_State* state);

int l_render(lua_State* state);