#include "mgr.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#define DEFAULT_WINDOW_COLOR 0xF48FB1
#define DEFAULT_FRAME_COLOR 0xF48FB1
/*
pipe: add request -> add logic agent -> frames it -> couples frame and logic agent -> displays it
TODO:
    - update workspace struct / init_workspace
    - rewrite frame function
    - update add function (kinda just rewrite it)
    - create delete function
logicTests.c

*/
//sets up workspace 
void init_workspace(struct WorkSpace* workspace, Display* display, Window root){
    //grabs the screen's atributes
    XWindowAttributes screenAttributes; 
    XGetWindowAttributes(display, root, &screenAttributes);
    
    struct LogicMaster* lm = (struct LogicMaster*) malloc(sizeof(struct LogicMaster));
    lm->root = NULL;
    lm->min_cord[0] = 0;
    lm->min_cord[1] = 0;
    lm->max_cord[0] = screenAttributes.width;
    lm->max_cord[1] = screenAttributes.height;

    workspace->logic_master = lm; 

    //sets up workspace root
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
}

//rule: split cur index(passed in through indexX and Y) along the longest edge 
void add_window_to_workspace(struct WorkSpace* workspace, Display* display, Window to_add, struct WindowFrame* cur_frame){
    struct WindowFrame* to_add_frame = (struct WindowFrame*) malloc(sizeof(struct WindowFrame));
    to_add_frame->la = logic_add(workspace->logic_master, cur_frame->la);
    frame_window(to_add_frame, workspace, to_add, display);
    //TODO update all edited children 
}

void frame_window(struct WindowFrame* frame, struct WorkSpace* workspace, Window to_add, Display* display){
    int x = frame->la->start_cord[0];
    int y = frame->la->start_cord[1];
    int width = frame->la->end_cord[0] - frame->la->start_cord[0]; 
    int height = frame->la->end_cord[1] - frame->la->start_cord[1]; 

    frame->frame = XCreateSimpleWindow(
            display,
            workspace->root,
            x,
            y,
            width,
            height,
            10,
            DEFAULT_FRAME_COLOR, //border color
            DEFAULT_FRAME_COLOR);

    XSelectInput(
            display,
            frame->frame,
            SubstructureRedirectMask | SubstructureNotifyMask);
  
    //puts the window in the frame 
    XReparentWindow(
            display,
            to_add,
            frame->frame,
            0, 0);  

    XMapWindow(display, frame->frame); //NOTE this will not display until the parent is displayed
}
