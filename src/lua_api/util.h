#include <luajit-2.1/lua.hpp>
#include <string>
#include <sstream>
#pragma once

void register_c_function(lua_State* state, char const * const tableName, char const * const funcName, lua_CFunction funcPointer);

void replace_all(
    std::string& s,
    std::string const& toReplace,
    std::string const& replaceWith
);