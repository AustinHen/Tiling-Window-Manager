#include "../include/mgr.h"
#include <sys/types.h>
#include <unistd.h>
int main(int argc, char** argv){
    //set up the manager
    Display* display;
    Window root;
    init_mgr(&display, &root); 
    //run the program
    mgr_event_loop(display, root);
    return 0;
}
