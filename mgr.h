#include <X11/Xlib.h>
#include <stdlib.h>
 
//a workspace is just a glorified root
struct WorkSpace{
    Window* root_;
    int* sizesX; // array of window sizes for x
    int* sizesY; // window sizes for y
    int width; // # windows across
    int height; // # windows up/down
    int * windows; // flattened 2d array that denotes where windows should be on the screen

};


int init_mgr(Display** display, Window* root);
void mgr_event_loop();
void mgr_error_handler();
