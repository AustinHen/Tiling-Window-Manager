#include "mgr.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#define DEFAULT_WINDOW_COLOR 0xF48FB1
//sets up workspace 
void init_workspace(struct WorkSpace* workspace, Display* display, Window root){
    //sets up workspace's fields (all empty) 
    workspace->width = 0; //no windows so the width is 0 
    workspace->height = 0; 

    //sets up workspace root
    //grabs the screen's atributes
    XWindowAttributes screenAttributes; 
    XGetWindowAttributes(display, root, &screenAttributes);

    //sets up window
    workspace->root = XCreateSimpleWindow(
            display,
            root,
            0, //puts window at the top left of the screen
            0,
            screenAttributes.width, //same width / height as the screen 
            screenAttributes.height,
            0,
            DEFAULT_WINDOW_COLOR,
            DEFAULT_WINDOW_COLOR);

    XSelectInput(
            display,
            workspace->root,
            SubstructureRedirectMask | SubstructureNotifyMask);
    //XAddToSaveSet(display, workspace->root);
}
