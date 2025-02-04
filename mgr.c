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
    int cur_workspace_index = 0; 

    //inits all workspaces
    for(int i=0; i<NUM_WORKSPACES; i++){
        init_workspace(&WorkSpaces[i], display, root);
    }

    //makes cur_workspace visible
    XMapWindow(display, (WorkSpaces[cur_workspace_index].root));

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
                handleMapRequest(display, &WorkSpaces[cur_workspace_index], curEvent.xmaprequest);
                break;

            case ReparentNotify:
                //we dont care -> we prob triggered it 
                break;

            case MapNotify:
                //we dont care -
                break;

            case ConfigureNotify:
                //we dont care
                break;

            case UnmapNotify:
                //TODO
                //- unmap frame -> reparent client window (so it doenst break ) -> xdestroywindow(Frame)
                //logic: just call logic delete window 
                break;

            case ButtonPress:
                //TODO
                //handle any button presses 
                //NOTE we have to write XGrabButton to propigate it upwards to the twm
                break;

            case ButtonRelease:
                //TODO
                break;

            case KeyPress:
                //TODO
                break;
                
            case KeyRelease:
                //TODO
                break;

            default:
                //unhandled event
                break;
        }
    }
}

//event handlers 
void handleConfigureRequest(Display* display, Window root, XConfigureRequestEvent event){
    //maybe do something but prob dont 
     

}

void handleMapRequest(Display* display, struct WorkSpace* workspace, XMapRequestEvent event){
    printf("here");
    struct WindowFrame* cur_focus = workspace->logic_master->cur_focus == NULL ? NULL : workspace->logic_master->cur_focus->window_frame;
    add_window_to_workspace(workspace, display, event.window, cur_focus);
}

void handleUnmapNotify(Display* display, struct WorkSpace* workspace, XUnmapEvent event){
    //1: search for the frame
    //2: unmap the frame
    //3: reparent client window 
    //
    //- unmap frame -> reparent client window (so it doenst break ) -> xdestroywindow(Frame)
    //logic: just call logic delete window 
}
