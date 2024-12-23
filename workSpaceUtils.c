#include "mgr.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#define DEFAULT_WINDOW_COLOR 0xF48FB1
#define DEFAULT_FRAME_COLOR 0xF48FB1
//sets up workspace 
void init_workspace(struct WorkSpace* workspace, Display* display, Window root){
    //grabs the screen's atributes
    XWindowAttributes screenAttributes; 
    XGetWindowAttributes(display, root, &screenAttributes);

    //sets up workspace's fields (all empty) 
    workspace->numWindows = 0;
    workspace->width = 0; //no windows so the width is 0 
    workspace->height = 0; 

    //TODO find a better way of doing this 
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
void add_window_to_workspace(struct WorkSpace* workspace, Display* display, Window to_add, struct WindowFrame* cur_frame){
    //if first window can skip most of this
    if(cur_frame == NULL){
        //add_first_window();
        return;
    }

    struct WindowFrame* to_add_frame = malloc(sizeof(struct WindowFrame));

    //figure out split direction
    int sizePxX = get_window_size_px(workspace->sizesX, cur_frame->rangeX);
    int sizePxY = get_window_size_px(workspace->sizesY, cur_frame->rangeY);
    //0 means split along x, 1 means split across y
    int splitDir = sizePxX > sizePxY ? 1 : 0;
    int *splitRange = splitDir ? cur_frame->rangeX : cur_frame->rangeY;
    struct sizes *sizesToUpdate = splitDir ? workspace->sizesX : workspace->sizesY;

    //splits the cur node 
    int half = (splitRange[1] - splitRange[0]) / 2;
    split_cur_node(splitRange, splitDir, sizesToUpdate, workspace, cur_frame, to_add_frame);

   //re updates split node 
    /*
    XWindowChanges changes;
    changes.x = curFrame->; //just the same x and y
    changes.y = curFrame->;
    changes.height = ; //dif height / width depending on split dir NOTE they cant be 0 so if they are just set tot 1
    changes.width = ;
    changes.border_width = ; //same
    changes.sibling = ; 
    changes.stack_mode = ; //who cares -> tiling means non stacking so this doesnt matter
    XConfigureWindow(display_, frame, e.value_mask, &changes); 
    //frame new window
    frame_window(to_add_frame, workspace, to_add, display);
    */
    //adds new window to workstation's lists
    to_add_frame->next = workspace->windows;
    workspace->windows = to_add_frame;
 
}

void frame_window(struct WindowFrame* frame, struct WorkSpace* workspace, Window to_add, Display* display){
    int x = get_index_sizes(workspace->sizesX, frame->rangeX[0]); //just the first value of range
    int y = get_index_sizes(workspace->sizesY, frame->rangeY[0]);
    int width = get_window_size_px(workspace->sizesX, frame->rangeX);
    int height = get_window_size_px(workspace->sizesY, frame->rangeY);

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

int get_window_size_px(struct sizes* s, int indexs[2]){
    return get_index_sizes(s, indexs[1]) - get_index_sizes(s, indexs[0]);
}

//translates a size index to a pixel value (ex index 0 would translate to 0px) returns -1 on failure
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


//updates the indexes of all frames to account for split node 
void split_node_indexes_adj(struct WindowFrame* head, struct WindowFrame* doNotUpdate, int index, int direction){
    //RULE: if index less than do nothing else update
    struct WindowFrame* cur = head;
    if(direction){
        //need to update x
        while(cur != NULL){
            if(cur != doNotUpdate){
                cur->rangeX[0] = cur->rangeX[0] >= index ? cur->rangeX[0]+1 : cur->rangeX[0];
                cur->rangeX[1] = cur->rangeX[1] >= index ? cur->rangeX[1]+1 : cur->rangeX[1];
            }
            cur = cur->next;
        } 
    }else{
        //need to update y
        while(cur != NULL){
            if(cur != doNotUpdate){
                cur->rangeY[0] = cur->rangeY[0] >= index ? cur->rangeY[0]+1 : cur->rangeY[0];
                cur->rangeY[1] = cur->rangeY[1] >= index ? cur->rangeY[1]+1 : cur->rangeY[1];
            }
            cur = cur->next;
        } 

    } 
}


void split_cur_node(int splitRange[2], int splitDir, struct sizes* sizesToUpdate, struct WorkSpace* workspace, struct WindowFrame* cur_frame, struct WindowFrame* to_add_frame){
    int half = (splitRange[1] - splitRange[0]) / 2;
    if((splitRange[1] - splitRange[0]) % 2 == 1){
        //split middle node 
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
        split_node_indexes_adj(workspace->windows, cur_frame, middleNode, splitDir);

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
}
