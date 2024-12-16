#include <X11/Xlib.h>
#include <stdlib.h>
 
//a workspace is just a glorified root
struct WorkSpace{
    Window* root;
    int* sizesX; // array of window sizes for x
    int* sizesY; // window sizes for y
    int width; // # windows across
    int height; // # windows up/down
    WindowFrame * windows; //windows in the workstation 
};

//handler for windows
struct WindowFrame{
    Window* frame;
    //denotes the indexes in the workspace grid where the window lies
    int rangeX[2]; //rangeX[0] < rangeX[1]
    int rangeY[2]; 
}

int init_mgr(Display** display, Window* root);
void mgr_event_loop();
void mgr_error_handler();
