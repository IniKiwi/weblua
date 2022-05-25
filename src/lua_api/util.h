#include <luajit-2.1/lua.hpp>
#include <string>
#include <sstream>
#include <iostream>
#pragma once

void register_c_function(lua_State* state, char const * const tableName, char const * const funcName, lua_CFunction funcPointer);

void replace_all(
    std::string& s,
    std::string const& toReplace,
    std::string const& replaceWith
);

void lshow_error(lua_State* state, std::string reason);