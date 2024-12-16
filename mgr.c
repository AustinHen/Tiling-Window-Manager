#include "mgr.h"
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
    //It seems the standard for event loops is for(;;). 
    //This is because while(1) does 1==1 and for(;;) doesnt do any comarison. How cool
    //I saved like three lines of assembly per loop
    for(;;){ 

    }
}

//logs error and exits
void mgr_error_handler(){
    printf("error");     
    fflush(stdout); //ensures the error message is visible
    exit(1); //exits with error
}
