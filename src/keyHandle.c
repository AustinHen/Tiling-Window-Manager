#include "../include/mgr.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <assert.h>
#include <stdio.h>
#include <X11/Xutil.h>
#include <sys/types.h>
#include<stdlib.h>
#include<unistd.h>
//handles all key presses
//just calls grab for all needed keys -> call on workstation root to get updated on all windows 
const unsigned int MM1 = Mod4Mask;
const unsigned int MM2 = Mod4Mask | ShiftMask;
void grab_all_keys(Display* display_, Window w){
    //grabs rest of em TODO, MAKE LESS jankye
    int keycodes_and_mods[31][2] = {
        {XKeysymToKeycode(display_ , XK_H), MM1},
        {XKeysymToKeycode(display_ , XK_J), MM1},
        {XKeysymToKeycode(display_ , XK_K), MM1},
        {XKeysymToKeycode(display_ , XK_L), MM1},
        {XKeysymToKeycode(display_ , XK_H), MM2},
        {XKeysymToKeycode(display_ , XK_J), MM2},
        {XKeysymToKeycode(display_ , XK_K), MM2},
        {XKeysymToKeycode(display_ , XK_L), MM2},
        {XKeysymToKeycode(display_ , XK_Q), MM2},
        {XKeysymToKeycode(display_ , XK_Return), MM1},
        {XKeysymToKeycode(display_ , XK_D), MM1},
        {XKeysymToKeycode(display_ , XK_0), MM1},
        {XKeysymToKeycode(display_ , XK_1), MM1},
        {XKeysymToKeycode(display_ , XK_2), MM1},
        {XKeysymToKeycode(display_ , XK_3), MM1},
        {XKeysymToKeycode(display_ , XK_4), MM1},
        {XKeysymToKeycode(display_ , XK_5), MM1},
        {XKeysymToKeycode(display_ , XK_6), MM1},
        {XKeysymToKeycode(display_ , XK_7), MM1},
        {XKeysymToKeycode(display_ , XK_8), MM1},
        {XKeysymToKeycode(display_ , XK_9), MM1},
        {XKeysymToKeycode(display_ , XK_0), MM2},
        {XKeysymToKeycode(display_ , XK_1), MM2},
        {XKeysymToKeycode(display_ , XK_2), MM2},
        {XKeysymToKeycode(display_ , XK_3), MM2},
        {XKeysymToKeycode(display_ , XK_4), MM2},
        {XKeysymToKeycode(display_ , XK_5), MM2},
        {XKeysymToKeycode(display_ , XK_6), MM2},
        {XKeysymToKeycode(display_ , XK_7), MM2},
        {XKeysymToKeycode(display_ , XK_8), MM2},
        {XKeysymToKeycode(display_ , XK_9), MM2},
    };

    for(int i=0; i<(sizeof(keycodes_and_mods))/sizeof(keycodes_and_mods[0]); i++){
        XGrabKey(display_,
                keycodes_and_mods[i][0], //keycode
                keycodes_and_mods[i][1], //mod
                w, //window
                0, //dont notify children
                GrabModeAsync,
                GrabModeAsync);
    }
}

//all actions to take TODO move to dif file
void swap_workspace(Display* display, int *cur_workspace_index, int swap_to_idx, struct WorkSpace *workspaces){
   //assert(swap_to_idx >= 0 && swap_to_idx < 10);

    //closes cur ws
    XUnmapWindow(display, workspaces[*cur_workspace_index].root);
    
    //opens new ws
    XMapWindow(display, (workspaces[swap_to_idx].root));
    *cur_workspace_index = swap_to_idx;
    //update focus
    
    if(workspaces[swap_to_idx].logic_master->cur_focus == NULL){
        //no windows just set 
        XSetInputFocus(display, workspaces[swap_to_idx].root, RevertToNone, CurrentTime);
    }else{
        //this looks soo bad
        XSetInputFocus(display,workspaces[swap_to_idx].logic_master->cur_focus->window_frame->w, RevertToNone, CurrentTime);

    }
}

//LOGIC
void swap_windows(int split_dir, int dir, Display* display, struct WorkSpace* workspace){
    struct LogicAgent* cur_focus = workspace->logic_master->cur_focus;
    struct LogicAgent* to_swap = get_focus_frame(split_dir, dir, cur_focus, workspace->logic_master);
    if(to_swap == NULL){
        return; //should have something to swap with
    }

    // just make logic agents point to dif windows 
    struct WindowFrame* to_swap_wf = to_swap->window_frame;
    struct WindowFrame* cur_wf = cur_focus->window_frame;

    to_swap->window_frame = cur_wf;
    cur_focus->window_frame = to_swap_wf;
    cur_wf->la = to_swap; 
    to_swap_wf->la = cur_focus; 

    //update everything cuz im lazy
    update_all_children_frames(workspace->logic_master->root, display);

    //update the logic focus to stay the same
    workspace->logic_master->cur_focus = to_swap;
}


void move_window_to_workspace(){}

void close_cur_window(Display* display, Window w){
    printf("window to delete: %d", w);
    XKillClient(display, w);
}


void open_app_launcher(){
    pid_t pid = fork();
    if(pid < 0){
        printf("failed to launch fork issue");
    }
    if(pid == 0){
        execl("/usr/bin/dmenu_run", "dmenu_run", (char*)NULL); 
        printf("failed to launch");
    }
}

void open_terminal(){
    pid_t pid = fork();
    if(pid < 0){
        printf("failed to launch fork issue");
    }
    if(pid == 0){
        execl("/usr/bin/xterm", "xterm", (char*)NULL); 
        printf("failed to launch");
    }
}

//TODO update function sig to get all data
void handleKeyPress(Display* display, Window root, XKeyEvent event, struct WorkSpace *workspaces, int* cur_focus_idx){
    struct WorkSpace workspace = workspaces[*cur_focus_idx];
    //rmv window
    if((event.state == MM1) && (event.keycode == XKeysymToKeycode(display, XK_Q))){
        open_app_launcher();
    }
    if((event.state == MM2) && (event.keycode == XKeysymToKeycode(display, XK_Q))){
        if(workspace.logic_master->cur_focus != NULL){
            close_cur_window(display, workspace.logic_master->cur_focus->window_frame->w);
        }
        return;
    }
    
    //opens file launcher
    if((event.state == MM1) && (event.keycode == XKeysymToKeycode(display, XK_D))){
        open_app_launcher();
        return;
    }
    //open open_terminal
    if((event.state == MM1) && (event.keycode == XKeysymToKeycode(display, XK_Return))){
        open_terminal();
        return;
    }
    //for some reason they are not inorder so we cant just add stuff to KP_0
    int keycodes[10] = {XK_0, XK_1, XK_2, XK_3, XK_4, XK_5, XK_6, XK_7, XK_8, XK_9};
    //workspace operations 
    for(int i=0; i<10; i++){
        int key_code =  XKeysymToKeycode(display, keycodes[i]);
        if(event.keycode == key_code){
            if(event.state == MM1){
                swap_workspace(display, cur_focus_idx,  i, workspaces);
            }
            if(event.state == MM2) {
                printf("shift works ig");
                move_window_to_workspace();
            }
            return;
        }
    }

    
    //code , split_dir, dir 
    //split_dir  0 if split on x, 1 if split y 
    int keys_to_dir[4][3] = { 
        {XKeysymToKeycode(display, XK_H), 0, -1},
        {XKeysymToKeycode(display, XK_L), 0, 1},
        {XKeysymToKeycode(display, XK_J), 1, 1},
        {XKeysymToKeycode(display, XK_K), 1, -1}
    }; 
    for(int i=0; i<4; i++){
        if(event.keycode == keys_to_dir[i][0]){
            //correct keycode 
            if(event.state == MM1){
                printf("swapping focus");
                update_focus(keys_to_dir[i][1], keys_to_dir[i][2], display, &workspace);
            }
            if(event.state == MM2){
                printf("gona swap windows");
                swap_windows(keys_to_dir[i][1], keys_to_dir[i][2], display, &workspace);
            }
            return;
        }
    }

}


