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

//logs error and exits
void mgr_error_handler(){
    printf("error");     
    fflush(stdout); //ensures the error message is visible
    exit(1); //exits with error
}

void mgr_event_loop(Display* display, Window root){
    XSelectInput(display, root, SubstructureRedirectMask | SubstructureNotifyMask); 
    //sets up WorkSpaces 
    const int NUM_WORKSPACES = 1;
    assert(NUM_WORKSPACES > 0 && NUM_WORKSPACES <= 10);

    struct WorkSpace WorkSpaces[NUM_WORKSPACES];
    int cur_workspace = 0; 

    //inits all workspaces
    for(int i=0; i<NUM_WORKSPACES; i++){
        init_workspace(&WorkSpaces[i], display, root);
    }

    //makes cur_workspace visible
    XMapWindow(display, (WorkSpaces[cur_workspace].root));

    //event loop
    XEvent curEvent;
    for(;;){ 
        //grabs nex event
        XNextEvent(display, &curEvent);

        //event handler
        switch(curEvent.type){
            case CreateNotify:
                //we dont care 
                break;

            case ConfigureRequest:
                //a window init's its size shape and other thing 
                //we will copy most things but when the window acutally gets displayed we will force it to be tiling
                handleConfigureRequest(display, root, curEvent.xconfigurerequest);
                break;
            
            case MapRequest:
                //request to make the window visible
                handleMapRequest(display, root, curEvent.xmaprequest);
                break;

            default:
                //unhandled event
                break;
        }
    }
}

//event handlers below
void handleConfigureRequest(Display* display, Window root, XConfigureRequestEvent event){
     

}

void handleMapRequest(Display* display, Window root, XMapRequestEvent event){
    add_window_to_workspace(workspace, display, to_add, cur_frame);

}



