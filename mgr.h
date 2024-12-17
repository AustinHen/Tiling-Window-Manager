#include <X11/Xlib.h>
#include <stdlib.h>
 
//a workspace is just a glorified root
struct WorkSpace{
    Window root;
    int* sizesX; // array of window sizes for x
    int* sizesY; // window sizes for y
    int width; // # windows across
    int height; // # windows up/down
    struct WindowFrame * windows; //windows in the workstation 
};

//handler for windows
struct WindowFrame{
    Window* frame;
    //denotes the indexes in the workspace grid where the window lies
    int rangeX[2]; //rangeX[0] < rangeX[1]
    int rangeY[2]; 
};

//mgr.c
int init_mgr(Display** display, Window* root);
void mgr_event_loop();
void mgr_error_handler();
void handleConfigureRequest(Display* display, Window root, XConfigureRequestEvent event);
void handleMapRequest(Display* display, Window root, XMapRequestEvent event);
//workSpaceUtils.c
void init_workspace(struct WorkSpace* workspace, Display* display, Window root);
