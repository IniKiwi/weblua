#include "util.h"
#include <server.h>

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

void replace_all(
	std::string& s,
	std::string const& toReplace,
	std::string const& replaceWith
) {
	std::ostringstream oss;
	std::size_t pos = 0;
	std::size_t prevPos = pos;

	while (true) {
		prevPos = pos;
		pos = s.find(toReplace, pos);
		if (pos == std::string::npos)
			break;
		oss << s.substr(prevPos, pos - prevPos);
		oss << replaceWith;
		pos += toReplace.size();
	}

	oss << s.substr(prevPos);
	s = oss.str();
}

void lshow_error(lua_State* state, std::string reason){
	lua_Debug ar;
	lua_getstack(state, 1, &ar);
	lua_getinfo(state, "nSl", &ar);
	int line = ar.currentline;
	std::string message =  "\e[34m";
	message += ar.source;
	message += "\e[39m line " + std::to_string(ar.currentline) + "\n" + reason;
	server::show_error("lua",message);

}

std::string percent_format(std::string input){
	std::string tdata = input;
	replace_all(tdata,"%0D%0A","\n");
	replace_all(tdata,"%0D","\n");
	replace_all(tdata,"%0A","\n");
	replace_all(tdata,"+"," ");
	replace_all(tdata,"%20"," ");
	replace_all(tdata,"%22","\"");
	replace_all(tdata,"%2B","+");
	replace_all(tdata,"%21","!");
	replace_all(tdata,"%23","#");
	replace_all(tdata,"%24","$");
	replace_all(tdata,"%26","&");
	replace_all(tdata,"%27","'");
	replace_all(tdata,"%28","(");
	replace_all(tdata,"%29",")");
	replace_all(tdata,"%2A","*");
	replace_all(tdata,"%2C",",");
	replace_all(tdata,"%2F","/");
	replace_all(tdata,"%3A",":");
	replace_all(tdata,"%3B",";");
	replace_all(tdata,"%3D","=");
	replace_all(tdata,"%3F","?");
	replace_all(tdata,"%40","@");
	replace_all(tdata,"%5B","[");
	replace_all(tdata,"%5D","]");
	replace_all(tdata,"%20"," ");
	replace_all(tdata,"%2D","-");
	replace_all(tdata,"%2E",".");
	replace_all(tdata,"%3C","<");
	replace_all(tdata,"%3E",">");
	replace_all(tdata,"%5C","\\");
	replace_all(tdata,"%5E","^");
	replace_all(tdata,"%5F","_");
	replace_all(tdata,"%60","`");
	replace_all(tdata,"%7B","{");
	replace_all(tdata,"%7C","|");
	replace_all(tdata,"%7D","}");
	replace_all(tdata,"%7E","~");
	replace_all(tdata,"%E9","é");
	replace_all(tdata,"%E8","è");
	replace_all(tdata,"%E7","ç");
	replace_all(tdata,"%E0","à");

	replace_all(tdata,"%25","%");
	return tdata;
}