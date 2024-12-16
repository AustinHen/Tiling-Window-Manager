#include "mgr.h"

int main(int argc, char** argv){
    //set up the manager
    Display* display;
    Window root;
    init_mgr(&display, &root); 
    //run the program
    mgr_event_loop(display, root);
}
