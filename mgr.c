#include "mgr.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
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
    //TODO sets error handler
    XSetErrorHandler(NULL);
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

    grab_all_keys(display, root);
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
                handleUnmapNotify(display, &WorkSpaces, NUM_WORKSPACES, curEvent.xunmap);
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
                handleKeyPress(display, root, curEvent.xkey);
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
    XWindowChanges changes;
    changes.x = event.x;
    changes.y = event.y;
    changes.width = event.width;
    changes.height = event.height;
    changes.border_width = event.border_width;
    changes.sibling = event.above;
    changes.stack_mode = event.detail;
    XConfigureWindow(display, event.window, event.value_mask, &changes);
}

void handleMapRequest(Display* display, struct WorkSpace* workspace, XMapRequestEvent event){
    printf("here");
    struct WindowFrame* cur_focus = workspace->logic_master->cur_focus == NULL ? NULL : workspace->logic_master->cur_focus->window_frame;
    add_window_to_workspace(workspace, display, event.window, cur_focus);
}

void handleUnmapNotify(Display* display, struct WorkSpace* workspaces, int num_ws, XUnmapEvent event){
    //1: find logic agent
    struct LogicAgent* to_remove_la = NULL; 
    int i =0;
    for(; i<num_ws; i++){
        to_remove_la = find_windows_la(workspaces[i].logic_master->root, event.window);
        if(to_remove_la != NULL){ break;}
    }
    if(to_remove_la == NULL){
        //prob one of our frames -> do nothing
        return;
    }
    //2: unmap the frame and kill the frame
    struct WindowFrame* to_remove_wf = to_remove_la->window_frame;
    XUnmapWindow(display, to_remove_wf->frame);
    XDestroyWindow(display, to_remove_wf->frame); 

    //3:logic remove
    // TODO I think issue is here but idk
    struct LogicAgent* sibling = logic_remove_leaf(workspaces[i].logic_master, to_remove_la);
    //remove WindowFrame
    free(to_remove_wf);

    if(workspaces[i].logic_master->root == NULL){
        return;
    }
    //XReparentWindow(display, event.window, workspaces[i].root, 0, 0);
    //redraw all bs
    update_all_children_frames(workspaces[i].logic_master->root, display);
    //get new focus 
    update_focus(0, 0, display, &workspaces[i]);

}
