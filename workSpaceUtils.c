#include "mgr.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#define DEFAULT_WINDOW_COLOR 0xF48FB1
//sets up workspace 
void init_workspace(struct WorkSpace* workspace, Display* display, Window root){
    //grabs the screen's atributes
    XWindowAttributes screenAttributes; 
    XGetWindowAttributes(display, root, &screenAttributes);

    //sets up workspace's fields (all empty) 
    workspace->numWindows = 0;
    workspace->width = 0; //no windows so the width is 0 
    workspace->height = 0; 

    struct sizes* x1 = malloc(sizeof(struct sizes));
    struct sizes* x2 = malloc(sizeof(struct sizes));
    struct sizes* y1 = malloc(sizeof(struct sizes));
    struct sizes* y2 = malloc(sizeof(struct sizes));

    x1->val = 0;
    x1->next = x2;
    x2->val = screenAttributes.width;

    y1->val = 0;
    y1->next = y2;
    y2->val = screenAttributes.height;

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
void add_window_to_workspace(struct WorkSpace* workspace, Display* display, Window to_add, struct WindowFrame* cur){
    //if first window can skip most of this
    if(cur == NULL){
        //add_first_window();
        return;
    }

    struct WindowFrame* to_add_frame = malloc(sizeof(struct WindowFrame));

    //figure out split direction
    int sizePxX = get_window_size_px(workspace->sizesX, cur->rangeX);
    int sizePxY = get_window_size_px(workspace->sizesY, cur->rangeY);
    //0 means split along x, 1 means split across y
    int splitDir = sizePxX > sizePxY ? 1 : 0;
    int *splitRange = splitDir ? cur->rangeX : cur->rangeY;
    struct sizes *sizesToUpdate = splitDir ? workspace->sizesX : workspace->sizesY;

    //if the size is odd we have to split the middle node
    int half = (splitRange[1] - splitRange[0]) / 2;
    if((splitRange[1] - splitRange[0]) % 2 == 1){
        //sizes update -> add an index to take half of the nodes 
        int middleNode = half + 1;
        struct sizes* cur = sizesToUpdate;
        int i = 0;
        while(cur != NULL && i++ < middleNode)
            cur = cur->next; 
        //cur = middle node
        struct sizes* splitNode = malloc(sizeof(struct sizes));
        splitNode->val = cur->val / 2;
        cur->val -= splitNode->val; // not div by 2 to prevent pixel los
        splitNode->next = cur->next;
        cur->next = splitNode;
        //update the nodes indexes 
    }

    //cuts cur window in half, giving the rest to the new node
    if(splitDir){
        //splitDir = X
        to_add_frame->rangeX[0] = half;
        to_add_frame->rangeX[1] = splitRange[1];

    }else{
        //splitDir = Y
        to_add_frame->rangeY[0] = half;
        to_add_frame->rangeY[1] = splitRange[1];
    }
    splitRange[1] = half;

    //re render split node 
    //frame new window
    frame_window(to_add_frame, to_add);

}

void frame_window(struct WindowFrame* frame, Window to_add){

}

int get_window_size_px(struct sizes* s, int indexs[2]){
    return get_index_sizes(s, indexs[1]) - get_index_sizes(s, indexs[0]);
}

//returns val OR -1 if not found 
int get_index_sizes(struct sizes* cur, int index){
    int cur_index = 0; 
    while(cur != NULL && cur_index < index){
        cur = cur->next;
        cur_index += 1;
    }

    if(cur == NULL)
        return -1; 

    return cur->val;
}
