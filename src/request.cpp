#include <request.h>
#include <fstream>
#include <iterator>
#include <sstream>
#include <vector>
#include <cstring>
#include <iomanip>


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
    type = words[0];
    if(route::exists(path) == true){
        route_type type = route::get(path)->get_type();
        if(type == route_type::PATH || type == route_type::PATH_CALLBACK){
            set_data_file(route::get(path)->get_file());
            mimetype = "text/html";
            status = "200 OK";
        }
    }
    else{
        set_data("404!");
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
    if(use_static_file){
        std::ifstream tfile;
        tfile.open(static_file, std::ios::binary);

        tfile.seekg(0, std::ios::end);
        size_t file_size = tfile.tellg();
        tfile.seekg(0, std::ios::beg);

        std::string header = "HTTP/1.1 " + status + "\n"+
        "Content-Type: "+mimetype+"\n"+
        "Content-Length: "+std::to_string(file_size)+"\n"+
        "Transfer-Encoding: chunked\n\n";
        write(client.sock,header.c_str(),header.length());
        char buffer[100020] = {0};
        size_t counter = 0;

        while(counter < file_size){
            if(file_size - counter < 100000){
                tfile.seekg(counter, std::ios::beg);
                std::stringstream stream;
                stream << std::hex << file_size-counter;

                size_t streamsize = strlen(stream.str().c_str());

                memcpy(&buffer[0],stream.str().c_str(),streamsize);
                memcpy(&buffer[streamsize],"\r\n",2);
                tfile.read(&buffer[streamsize+2],file_size - counter);
                memcpy(&buffer[streamsize+2+file_size-counter],"\r\n",2);
                write(client.sock, buffer, streamsize+2+file_size-counter+2);
                write(client.sock,"0\r\n\r\n",5);
                break;
            } else {
                std::string _log = "sending file..." + std::to_string(counter) + "/" + std::to_string(file_size);
                log(_log);
                tfile.seekg(counter, std::ios::beg);
                memset(&buffer[0],0,100020);
                memcpy(&buffer[0],"186A0\r\n",7);
                tfile.read(&buffer[7],100000);
                counter = counter+100000;
                memcpy(&buffer[100007],"\r\n",2);
                write(client.sock,buffer,100009);
            }
        }
    } else {
        std::string header = "HTTP/1.1 " + status + "\n"+
        "Content-Type: "+mimetype+"\n"+
        "Content-Length: "+std::to_string(data_size)+"\n\n";
        char buffer[header.length()+data_size+1] = {0};
        memcpy(&buffer[0], header.c_str(), header.length());
        memcpy(&buffer[header.length()], &data[0], data_size);
        write(client.sock,&buffer,sizeof(buffer));
        close(client.sock);
    }
    
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

void request::set_data(std::string _data){
    data.clear();
    data_size = _data.length();
    data.resize(data_size);
    memcpy(&data[0],_data.c_str(),_data.length());
}

void request::set_mimetype(std::string _mimetype){
    mimetype = _mimetype;
}

void request::set_status(std::string _status){
    status = _status;
}

std::string request::get_data(){
    return reinterpret_cast<char*>(data.data());
}
std::string request::get_mimetype(){
    return mimetype;
}
std::string request::get_status(){
    return status;
}
client_t request::get_client(){
    return client;
}
std::string request::get_type(){
    return type;
}

int request::set_data_file(std::string filename){
    std::ifstream tfile;
    tfile.open(filename, std::ios::binary);
    if(tfile.is_open()){
        tfile.seekg(0, std::ios::end);
        data_size = tfile.tellg();
        tfile.seekg(0, std::ios::beg);

        data.clear();
        data.resize(data_size);

        tfile.read(reinterpret_cast<char*>(&data[0]), data_size);
        return 0;
    }
    return 1;
}

void request::set_static_file(std::string _file){
    static_file = _file;
}
void request::set_use_static_file(bool _use){
    use_static_file = _use;
}

void request::log(std::string str){
    std::cout << std::to_string(id) << "> " << str << "\n";
}