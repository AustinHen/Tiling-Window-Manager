#include "../include/mgr.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>

FILE* log_ptr;
char* LOG_NAME = (char*) "log.txt";

int open_log(){
    log_ptr = fopen(LOG_NAME, "a");
    return log_ptr == NULL;
}

void close_log(){
   fclose(log_ptr); 
}

void write_log(const char* msg){
    open_log();
    fprintf(log_ptr, msg); 
    close_log();

}

//i think it has to be an int return -> return val means nothing
int log_error(Display* display, XErrorEvent* error) { 
    open_log();
    fprintf(log_ptr, "error: %d", error->error_code);
    close_log();
    return 1;
}

