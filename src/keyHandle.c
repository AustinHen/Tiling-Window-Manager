#include "../include/mgr.h"
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
    //grabs all of the mod -> numbers
    for(int i=0; i<10; i++){
        int key_code =  XKeysymToKeycode(display_, XK_KP_0 + i);
        XGrabKey(display_,
                key_code, //keycode
                MM1, //mod
                w, //window
                0, //dont notify children
                GrabModeAsync,
                GrabModeAsync);

         XGrabKey(display_,
                key_code, //keycode
                MM2, //mod
                w, //window
                0, //dont notify children
                GrabModeAsync,
                GrabModeAsync);
 
    }
    
    //grabs rest of em TODO, MAKE LESS jankye
    int keycodes_and_mods[11][2] = {
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
        {XKeysymToKeycode(display_ , XK_D), MM1}
    };

    for(int i=0; i<(sizeof(keycodes_and_mods))/sizeof(keycodes_and_mods[0]); i++){
        XGrabKey(display_,
                keycodes_and_mods[i][0], //keycode
                keycodes_and_mods[1][1], //mod
                w, //window
                0, //dont notify children
                GrabModeAsync,
                GrabModeAsync);
    }
}

//all actions to take TODO move to dif file
//void swap_workspace(int cur_workspace_index, int swap_to_idx, struct WorkSpace *workspaces){
   //assert(swap_to_idx >= 0 && swap_to_idx < 10);
//}
void move_window_to_workspace(){}

void close_cur_window(){}

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
void handleKeyPress(Display* display, Window root, XKeyEvent event, struct WorkSpace workspace){
    //rmv window
    if((event.state & MM2) && (event.keycode == XKeysymToKeycode(display, XK_Q))){
        exit(1);
        close_cur_window();
        return;
    }
    
    //opens file launcher
    if((event.state & MM1) && (event.keycode == XKeysymToKeycode(display, XK_D))){
        open_app_launcher();
        return;
    }
    //open open_terminal
    if((event.state & MM1) && (event.keycode == XKeysymToKeycode(display, XK_Return))){
        open_terminal();
        return;
    }
    
    //workspace operations 
    for(int i=0; i<10; i++){
        int key_code =  XKeysymToKeycode(display, XK_KP_0 + i);
        if(event.keycode == key_code){
            if(event.state & MM1){
                //swap_workspace();
            }
            if(event.state & MM2) {
                move_window_to_workspace();
            }
            return;
        }
    }

    
    //just arow keys lef
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
            if(event.state & MM1){
                update_focus(keys_to_dir[i][1], keys_to_dir[i][2], display, &workspace);
            }
            if(event.state & MM2){
            }
            return;
        }
    }

}


