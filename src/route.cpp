#include "route.h"
std::map<std::string,route*> route::routes;

route::route(route_type _type, std::string _path){
    type = _type;
    route::routes.insert(std::pair<std::string,route*>(_path,this));
}

route::~route(){
    route::routes.erase(path);
    std::cout << "deleted route.\n";
}

route* route::get(std::string _path){
    return route::routes.at(_path);
}

bool route::exists(std::string _path){
    if(route::routes.find(_path) == route::routes.end()){
        return false;
    }
    return true;
}

void route::callback(lua_State* state, uint32_t _id){
    lua_rawgeti(state, LUA_REGISTRYINDEX, r_callback);
    lua_pushnumber(state,_id);
    lua_call(state, 1, 0);
}

void route::set_file(std::string _file){
    file = _file;
}
void route::set_lua_callback(int _callback){
    r_callback = _callback;
}

std::string route::get_file(){
    return file;
}
int route::get_lua_callback(){
    return r_callback;
}
route_type route::get_type(){
    return type;
}

