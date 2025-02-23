#include "../include/mgr.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

void update_focus(int split_dir, int dir, Display* display_, struct WorkSpace* workspace){
    //call update then shift focus
    printf("MEEEP: %d", workspace->logic_master->cur_focus);
    struct LogicAgent* next_focus = get_focus_frame(split_dir, dir, workspace->logic_master->cur_focus, workspace->logic_master);
    if(next_focus == NULL){
        //needs to have some focus
        XSetInputFocus(display_, workspace->root, RevertToNone, CurrentTime);
        return;
    }
    printf("hello : %d \n", next_focus);
    workspace->logic_master->cur_focus = next_focus;
    XSetInputFocus(display_, next_focus->window_frame->w, RevertToNone, CurrentTime);
}

//LOGIC
//dir -> eg left or right | up or down
struct LogicAgent* get_focus_frame(int split_dir, int dir, struct LogicAgent* cur_focus, struct LogicMaster* ws){
    if(cur_focus == NULL){
        return get_default_cur_focus(ws);
    }

    struct LogicAgent* top = get_focus_frame_bubble_up(split_dir, dir, cur_focus);
    if(top == NULL){
        //cur_focus is the furthest frame in the set direction 
        printf("\n no TOP \n");
        return cur_focus;
    }
    return get_focus_frame_bubble_down(split_dir, dir, top); 
}

struct LogicAgent* get_focus_frame_bubble_up(int split_dir, int dir, struct LogicAgent* cur_focus){
    struct LogicAgent* cur = cur_focus;
    struct LogicAgent* prev = cur;
    while(cur != NULL && cur->parent!=NULL){
        prev = cur;
        cur = cur->parent;
        if(cur->split_dir != split_dir){
            continue; //not the right split dir -> keep going up
        }
        struct LogicAgent* not_prev = cur->left != prev ? cur->left : cur->right; 
        printf("inloop: %d", not_prev);

        int dif = prev->start_cord[split_dir] - not_prev->start_cord[split_dir];
        if(dif * dir < 0){
            return not_prev;
        }
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

        //TODO test logic 
        //Take step opp to dir
        int dif = cur->left->start_cord[split_dir] - cur->right->start_cord[split_dir];
        if(dif * dir > 0){
            cur = cur->left;
        }else{
            cur = cur->right;
        }
    }
    assert(cur!=NULL);
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
