#include "server.h"

int main(int argc, char *argv[]){
    server::init(80);
    server::init_lua();
    server::main_loop();
}