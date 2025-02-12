#include "mgr.h"
#include <stdio.h>
#include <X11/Xutil.h>
//handles all key presses
//just calls grab for all needed keys -> call on workstation root to get updated on all windows 
void grab_all_keys(Display* display_, Window w){
    const unsigned int MM1 = Mod4Mask;
    const unsigned int MM2 = Mod4Mask | ShiftMask;

    //need to XGrabEvent() to allow futher processing
    int keycodes_and_mods[1][2] = {
        {XKeysymToKeycode(display_ , XK_A), MM1}
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

//TODO update function sig to get all data
void handleKeyPress(Display* display, Window root, XKeyEvent event){
     printf("i got pressed");
     exit(1);
}
