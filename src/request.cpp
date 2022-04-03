#include <request.h>
#include <fstream>
#include <iterator>
#include <sstream>
#include <vector>
#include <cstring>
#include <iomanip>
#include <cstdlib>
#include <lua_api/util.h>


std::map<uint32_t,request*> request::requests;
uint32_t request::request_counter;
std::mutex request::mutex;

request::request(lua_State* state, client_t _client){
    client = _client;
    ended = false;
    request::mutex.lock();
    request::requests.insert(std::pair<uint32_t,request*>(request::request_counter,this));
    id = request::request_counter;
    request::request_counter++;
    char* buffer;
    buffer = (char*)malloc(50000); 
    read(_client.sock, buffer, 50000);
    request::mutex.unlock();

    client_request = buffer;
    free(buffer);

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
            set_static_file(route::get(path)->get_file());
            mimetype = route::get(path)->get_mimetype();
            status = "200 OK";
        }
    }
    else{
        set_data("404!");
        mimetype = "text/plain";
        status = "404 Not Found";
    }
    std::cout << "\r\e[39m" << std::to_string(id) << ": " << words[0] << " \e[93m" << path << " \e[39m" << status << "\e[90m\n> ";
    if(route::exists(path) == true){
        route_type type = route::get(path)->get_type();
        if(type == route_type::CUSTOM || type == route_type::PATH_CALLBACK){
            request::mutex.lock();
            callback(state);
            request::mutex.unlock();
        }
    }

    if(ended == false){
        send();
    }
    
}

void request::http_redirect(std::string _loc){
    ended = true;
    std::string header = "HTTP/1.1 301 Moved Permanently\r\nConnection: close\r\nLocation: "+_loc+"\r\n\r\n";
    write(client.sock,header.c_str(),header.length());
}

void request::send(){
    if(use_static_file){
        std::ifstream tfile;
        tfile.open(static_file, std::ios::binary);

        tfile.seekg(0, std::ios::end);
        size_t file_size = tfile.tellg();
        tfile.seekg(0, std::ios::beg);

        if(file_size > WEBLUA_MAX_FILE_SIZE){
            return;
        }

        std::string header = "HTTP/1.1 " + status + "\r\n"+
        "Content-Type: "+mimetype+"\r\n"+
        "Content-Length: "+std::to_string(file_size)+"\r\n"+
        "Transfer-Encoding: chunked\r\n"+
        "Connection: close\r\n\r\n";
        write(client.sock,header.c_str(),header.length());
        char* buffer;
        buffer = (char*) malloc(WEBLUA_CHUNKED_PACKET_SIZE + 20);
        size_t counter = 0;

        while(counter < file_size){
            if(file_size - counter < WEBLUA_CHUNKED_PACKET_SIZE){
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
                ended = true;
                tfile.close();
                free(buffer);
                close(client.sock);
                break;
            } else {
                std::string _log = "sending file..." + std::to_string(counter) + "/" + std::to_string(file_size);
                log(_log);
                tfile.seekg(counter, std::ios::beg);
                memset(&buffer[0],0,WEBLUA_CHUNKED_PACKET_SIZE+20);
                std::stringstream stream;
                stream << std::hex << WEBLUA_CHUNKED_PACKET_SIZE;
                std::string result( stream.str() );
                result += "\r\n";
                memcpy(&buffer[0],result.c_str(),result.length());
                tfile.read(&buffer[7],WEBLUA_CHUNKED_PACKET_SIZE);
                counter = counter+WEBLUA_CHUNKED_PACKET_SIZE;
                memcpy(&buffer[WEBLUA_CHUNKED_PACKET_SIZE+7],"\r\n",2);
                write(client.sock,buffer,WEBLUA_CHUNKED_PACKET_SIZE+9);
                
            }
        }
    } else {
        std::string header = "HTTP/1.1 " + status + "\r\n"+
        "Content-Type: "+mimetype+"\r\n"+
        "Content-Length: "+std::to_string(data_size)+"\r\nConnection: close\r\n\r\n";
        char buffer[header.length()+data_size+1] = {0};
        memcpy(&buffer[0], header.c_str(), header.length());
        memcpy(&buffer[header.length()], &data[0], data_size);
        write(client.sock,&buffer,sizeof(buffer));
        ended = true;
        close(client.sock);
    }
    
}

request::~request(){
    request::mutex.lock();
    request::requests.erase(id);
    request::mutex.unlock();
    client_request.clear();
    
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
    use_static_file = false;
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
    use_static_file = false;
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

void request::set_static_file(std::string _file){
    use_static_file = true;
    static_file = _file;
}
void request::set_use_static_file(bool _use){
    use_static_file = _use;
}

void request::log(std::string str){
    std::cout << "\e[90m\r\e[90m" << std::to_string(id) << "> " << str << "\e[39m\n> ";
}

 std::string request::get_form_field(std::string _field){
    if(type == "POST"){
        size_t form_start = client_request.find("\r\n\r\n");
        std::string form_data = client_request.substr(form_start+4,std::stoi(get_http_arg("Content-Length").c_str()));

        size_t feild_name_start = client_request.find(_field,form_start+4);
        if(feild_name_start == std::string::npos){
            return "";
        }
        size_t feild_data_start = client_request.find_first_of("=",feild_name_start);
        feild_data_start++;

        size_t feild_data_end = client_request.find_first_of("&",feild_name_start);
        if(feild_name_start == std::string::npos){
            return client_request.substr(feild_data_start, client_request.length()-feild_data_start);
        }

        std::string tdata = client_request.substr(feild_data_start, feild_data_end-feild_data_start);
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
    return "";
 }

 std::string request::get_http_arg(std::string _arg){
    size_t arg_start = client_request.find(_arg);
    size_t arg_data_start = client_request.find(": ",arg_start);
    size_t arg_end = client_request.find_first_of("\r",arg_start);
    return client_request.substr(arg_data_start+2, arg_end-arg_data_start);

 }