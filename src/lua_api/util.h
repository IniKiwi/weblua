#include <luajit-2.1/lua.hpp>
#pragma once

void register_c_function(lua_State* state, char const * const tableName, char const * const funcName, lua_CFunction funcPointer);