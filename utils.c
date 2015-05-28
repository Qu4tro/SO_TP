#include "utils.h"

void print_error_and_exit(){
    fprintf(stderr, "%s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

void print_and_exit(char* message){
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}


