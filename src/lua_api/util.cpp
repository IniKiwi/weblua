#include "util.h"

void register_c_function(lua_State* state, char const * const tableName, char const * const funcName, lua_CFunction funcPointer)
{
    lua_getfield(state, LUA_GLOBALSINDEX, tableName);  // push table onto stack
    if (!lua_istable(state, -1))                       // not a table, create it
    {
        lua_createtable(state, 0, 1);      // create new table
        lua_setfield(state, LUA_GLOBALSINDEX, tableName);  // add it to global context

        // reset table on stack
        lua_pop(state, 1);                 // pop table (nil value) from stack
        lua_getfield(state, LUA_GLOBALSINDEX, tableName);  // push table onto stack
    }

    lua_pushstring(state, funcName);       // push key onto stack
    lua_pushcfunction(state, funcPointer); // push value onto stack
    lua_settable(state, -3);               // add key-value pair to table

    lua_pop(state, 1);                     // pop table from stack
}