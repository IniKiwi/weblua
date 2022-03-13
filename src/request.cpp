#include <request.h>
#include <fstream>
#include <iterator>
#include <sstream>


std::map<uint32_t,request*> request::requests;
uint32_t request::request_counter;

request::request(lua_State* state, client_t _client){
    client = _client;
    request::requests.insert(std::pair<uint32_t,request*>(request::request_counter,this));
    id = request::request_counter;
    request::request_counter++;

    char buffer[50000] = {0}; 
    read(_client.sock, buffer, 50000);

    client_request = buffer;

    std::vector<std::string> words;
    std::istringstream stream(client_request);
    std::copy(
        std::istream_iterator<std::string>( stream ),
        std::istream_iterator<std::string>(),
        std::back_inserter( words )
    );
    path = words[1];
    if(route::exists(path) == true){
        route_type type = route::get(path)->get_type();
        if(type == route_type::PATH || type == route_type::PATH_CALLBACK){
            std::ifstream file;
            file.open(route::get(path)->get_file(), std::ios::binary);
            data = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            mimetype = "text/html";
            status = "200 OK";
        }
    }
    else{
        data = "404!";
        mimetype = "text/plain";
        status = "404 Not Found";
    }
    std::cout << "\e[39m" << std::to_string(id) << ": " << words[0] << " \e[93m" << path << " \e[39m" << status << "\e[90m\n";
    if(route::exists(path) == true){
        route_type type = route::get(path)->get_type();
        if(type == route_type::CUSTOM || type == route_type::PATH_CALLBACK){
            callback(state);
        }
    }
}

void request::send(){
    std::string ret = "HTTP/1.1 " + status + "\n"+
    "Content-Type: "+mimetype+"\n"+
    "Content-Length: "+std::to_string(data.length())+"\n\n"+data;
    write(client.sock,ret.c_str(),ret.length());
    close(client.sock);
}

request::~request(){
    request::requests.erase(id);
}

request* request::get(uint32_t _id){
    return request::requests.at(_id);
}

void request::rest_request_counter(){
    request::request_counter = 0;
}

void request::callback(lua_State* state){
    route::get(path)->callback(state, id);
}