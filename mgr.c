#include "mgr.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
//starts the manager
//returns 1 on suc, 0 on fail 
int init_mgr(Display** display, Window* root){
     (*display) = XOpenDisplay(NULL); 
     if((*display) == NULL){
        //did not open right
        return 0;
     }
     (*root) = DefaultRootWindow((*display));
     return 1;
        
}

void mgr_event_loop(Display* display, Window root){
    XSelectInput(display, root, SubstructureRedirectMask | SubstructureNotifyMask); 
    //sets up WorkSpaces : NOTE there is only one for now 
    const int NUM_WORKSPACES = 1;
    assert(NUM_WORKSPACES > 0 && NUM_WORKSPACES <= 10);

    struct WorkSpace WorkSpaces[NUM_WORKSPACES];
    int cur_workspace = 0; 

    //event loop
    XEvent curEvent;
    for(;;){ 
        //grabs nex event
        XNextEvent(display, &curEvent);

        //event handler
        switch(curEvent.type){
            
            default:
                //unhandled event
                break;
        }
    }
}

//logs error and exits
void mgr_error_handler(){
    printf("error");     
    fflush(stdout); //ensures the error message is visible
    exit(1); //exits with error
}



