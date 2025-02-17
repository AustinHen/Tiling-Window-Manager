#include "mgr.h"
#include <stdio.h>
#include <X11/Xutil.h>
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
    int keycodes_and_mods[10][2] = {
        {XKeysymToKeycode(display_ , XK_H), MM1},
        {XKeysymToKeycode(display_ , XK_J), MM1},
        {XKeysymToKeycode(display_ , XK_K), MM1},
        {XKeysymToKeycode(display_ , XK_L), MM1},
        {XKeysymToKeycode(display_ , XK_H), MM2},
        {XKeysymToKeycode(display_ , XK_J), MM2},
        {XKeysymToKeycode(display_ , XK_K), MM2},
        {XKeysymToKeycode(display_ , XK_L), MM2},
        {XKeysymToKeycode(display_ , XK_Q), MM2},
        {XKeysymToKeycode(display_ , XK_Return), MM1}
    };

    for(int i=0; i<(sizeof(keycodes_and_mods))/sizeof(keycodes_and_mods[0]); i++){
        XGrabKey(display_,
                keycodes_and_mods[i][0], //keycode
                MM2, //mod
                w, //window
                0, //dont notify children
                GrabModeAsync,
                GrabModeAsync);
    }
}

//all actions to take TODO move to diff file
void swap_workspace(){}
void move_window_to_workspace(){}
void change_focus(){}
void swap_windows(){}
void close_cur_window(){}
void open_terminal(){}


//TODO update function sig to get all data
void handleKeyPress(Display* display, Window root, XKeyEvent event){
    //rmv window
    if((event.state & MM2) && (event.keycode == XKeysymToKeycode(display, XF_Q))){
        close_cur_window();
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
            if(event & MM1){
                swap_workspace()
            }
            if(event & MM2) {
                move_window_to_workspace();
            }
            return;
        }
    }

    
    //just arow keys lef
    //code , split_dir, dir 
    //split_dir  0 if split on x, 1 if split y 
    int keys_to_dir[4][3]{ 
        {XKeysymToKeycode(display, XP_H), 0, -1},
        {XKeysymToKeycode(display, XP_L), 0, 1},
        {XKeysymToKeycode(display, XP_J), 1, -1},
        {XKeysymToKeycode(display, XP_K), 1, 1}
    } 
    for(int i=0; i<4; i++){
        if(event.keycode == keys_to_dir[i][0]){
            //correct keycode 
            if(event & MM1){
                change_focus();
            }
            if(event & MM2){
                swap_windows();
            }
            return;
        }
    }
    assert(false, "Unhandled grabed keycode");
}

