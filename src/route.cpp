#include "route.h"
std::map<std::string,route*> route::routes;

route::route(route_type _type, std::string _path){
    type = _type;
    route::routes.insert(std::pair<std::string,route*>(_path,this));
}

route::~route(){
    route::routes.erase(path);
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
    std::string ext = _file.substr(_file.find_last_of(".") + 1);
    if(ext == "html" || ext == "htm"){
        set_mimetype("text/html");
    }
    else if(ext == "txt"){
        set_mimetype("text/plain");
    }
    else if(ext == "png"){
        set_mimetype("image/x-png");
    }
    else if(ext == "jpeg" || ext == "jpg"){
        set_mimetype("image/jpeg");
    }
    else if(ext == "pdf"){
        set_mimetype("application/pdf");
    }
    else if(ext == "gif"){
        set_mimetype("image/gif");
    }
    else if(ext == "zip"){
        set_mimetype("application/zip");
    }
    else if(ext == "xml"){
        set_mimetype("application/xml");
    }
    else if(ext == "7z"){
        set_mimetype("application/x-7z-compressed");
    }
    else if(ext == "svg"){
        set_mimetype("image/svg+xml");
    }
    else if(ext == "rar"){
        set_mimetype("application/x-rar-compressed");
    }
    else if(ext == "json"){
        set_mimetype("application/json");
    }
    else if(ext == "js"){
        set_mimetype("application/javascript");
    }
    else if(ext == "ico"){
        set_mimetype("image/x-icon");
    }
    else if(ext == "css"){
        set_mimetype("text/css");
    }
    else if(ext == "csv"){
        set_mimetype("text/csv");
    }
    else if(ext == "bmp"){
        set_mimetype("image/bmp");
    }
    else if(ext == "avi"){
        set_mimetype("video/x-msvideo");
    }
    else if(ext == "mpeg"){
        set_mimetype("video/mpeg");
    }
    else if(ext == "webm"){
        set_mimetype("video/webm");
    }
    else{
        set_mimetype("application/octet-stream");
    }
}
void route::set_lua_callback(int _callback){
    r_callback = _callback;
}
void route::set_mimetype(std::string _mimetype){
    mimetype = _mimetype;
}

std::string route::get_file(){
    return file;
}
std::string route::get_mimetype(){
    return mimetype;
}
int route::get_lua_callback(){
    return r_callback;
}
route_type route::get_type(){
    return type;
}

