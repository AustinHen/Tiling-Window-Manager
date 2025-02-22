#include "../include/mgr.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>


void update_focus(int split_dir, int dir, Display* display_, struct WorkSpace* workspace){
    //call update then shift focus
    struct LogicAgent* next_focus = get_focus_frame(split_dir, dir, workspace->logic_master->cur_focus, workspace->logic_master);
    if(next_focus == NULL){
        //needs to have some focus
        XSetInputFocus(display_, workspace->root, RevertToNone, CurrentTime);
        return;
    }
    XSetInputFocus(display_, next_focus->window_frame->w, RevertToNone, CurrentTime);
}

//LOGIC
//dir -> eg left or right | up or down
struct LogicAgent* get_focus_frame(int split_dir, int dir, struct LogicAgent* cur_focus, struct LogicMaster* ws){
    if(cur_focus == NULL || split_dir == 0){
        return get_default_cur_focus(ws);
    }

    struct LogicAgent* top = get_focus_frame_bubble_up(split_dir, dir, cur_focus);
    if(top == NULL){
        //cur_focus is the furthest frame in the set direction 
        return cur_focus;
    }
    return get_focus_frame_bubble_down(split_dir, dir, cur_focus); 
}

struct LogicAgent* get_focus_frame_bubble_up(int split_dir, int dir, struct LogicAgent* cur_focus){
    struct LogicAgent* cur = cur_focus;
    struct LogicAgent* prev = cur;
    while(cur != NULL){
        prev = cur;
        cur = cur->parent;
        if(cur->split_dir != split_dir){
            continue; //not the right split dir -> keep going up
        }
        struct LogicAgent* not_prev = cur->left != prev ? cur->left : cur->right; 
        
        //TODO update logic so its not this bad looking
        if(prev->start_cord[split_dir] < not_prev->start_cord[split_dir]){
            if(dir < 0){
                continue;
            }
        }else{
            if(dir > 0){
                continue; 
            }
        }
        return cur; 
    }

    return NULL;
}


struct LogicAgent* get_focus_frame_bubble_down(int split_dir, int dir, struct LogicAgent* top){
    struct LogicAgent* cur = top;
    assert(cur != NULL); //how did we get here 
    while(cur->left != NULL){
        if(cur->split_dir != split_dir){
            //who cares
            cur = cur->left; 
            continue;
        }    
        //TODO FINISH AND CHECK LOGIC
    }
    return cur;
}

//returns a valid cur_focus
struct LogicAgent* get_default_cur_focus(struct LogicMaster* ws){
    struct LogicAgent* cur = ws->root;
    if(cur == NULL){
        return NULL;
    }
    //assert(cur != NULL); //there are no nodes -> how did we get here 

    while(cur->right != NULL || cur->left != NULL){
        if(cur->right != NULL){
            cur = cur->right;
        }else{
            cur = cur->left;
        }
    }
    return cur;
}
