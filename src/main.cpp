#include "server.h"

int main(int argc, char *argv[]){
    if(argc == 2){
        server::init(80);
        server::init_lua(argv[1]);
        server::main_loop();
    }   
}