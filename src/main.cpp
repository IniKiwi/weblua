#include "server.h"

int main(int argc, char *argv[]){
    if(argc == 2){
        server::init(80, argv[1]);
    }   
}