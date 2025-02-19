#include "../include/mgr.h"
#include <stdio.h>
#include <assert.h>

struct LogicAgent* logic_add(struct LogicMaster* ws, struct LogicAgent* cur_focus){
    //sets up the basic format of the new agent
    struct LogicAgent* to_add = (struct LogicAgent*) malloc(sizeof(struct LogicAgent));
    to_add->left = NULL;
    to_add->right = NULL;
    to_add->parent = NULL;
    to_add->window_frame = NULL;

    if(ws->root == NULL){
        //this is the first agent -> can just add it 
        to_add->start_cord[0] = ws->min_cord[0];
        to_add->start_cord[1] = ws->min_cord[1];
        to_add->end_cord[0] = ws->max_cord[0];
        to_add->end_cord[1] = ws->max_cord[1];
        ws->root = to_add;
    }else{
        //need to split some node to fit this in
        if(cur_focus == NULL){
            cur_focus = get_default_cur_focus(ws);
        }

        logic_split(ws, cur_focus, to_add); 
    }
    ws->cur_focus = to_add;
    return to_add; 
}

//splits to_split, giving half its space to to_add and reparents both nodes
void logic_split(struct LogicMaster* ws, struct LogicAgent* to_split, struct LogicAgent* to_add){
    assert(to_split != NULL); //to split should be a node 
    assert(to_split->right == NULL && to_split->left == NULL); //to split should be a leaf

    int split_dir = get_split_dir(to_split);
    int non_split_dir = (split_dir + 1) % 2;

    //makes the parent of to_add and to_split; yes there is a lot of copying 
    struct LogicAgent* new_parent = (struct LogicAgent*) malloc(sizeof(struct LogicAgent));
    new_parent->window_frame = NULL;
    new_parent->parent = to_split->parent;
    new_parent->split_dir = split_dir;
    if(new_parent->parent != NULL){
        if(new_parent->parent->left == to_split){
            new_parent->parent->left = new_parent;
        }else{
            new_parent->parent->right = new_parent;
        }
    }else{
        //new parent is the new root
        ws->root = new_parent;
    }
    to_split->parent = new_parent;
    new_parent->left = to_split;
    new_parent->right = to_add;
    new_parent->start_cord[0] = to_split->start_cord[0]; 
    new_parent->start_cord[1] = to_split->start_cord[1]; 
    new_parent->end_cord[0] = to_split->end_cord[0]; 
    new_parent->end_cord[1] = to_split->end_cord[1]; 

    to_add->parent = new_parent;

    //non split dir -> just copy from to_split 
    to_add->start_cord[non_split_dir] = to_split->start_cord[non_split_dir];
    to_add->end_cord[non_split_dir] = to_split->end_cord[non_split_dir];

    //split dir -> cut to_split in half (duh idk why im writing this)
    int middle = (to_split->end_cord[split_dir] - to_split->start_cord[split_dir]) / 2 + to_split->start_cord[split_dir];
    to_add->end_cord[split_dir] = to_split->end_cord[split_dir];
    to_split->end_cord[split_dir] = middle;
    to_add->start_cord[split_dir] = middle;
}

//returns 0 if split on x, 1 if split y 
int get_split_dir(struct LogicAgent* cur_focus){
    int depth = get_depth(cur_focus);
    assert(depth != -1);//should be able to find cur_focus
    return depth % 2;
}

int get_depth(struct LogicAgent* cur_focus){
    assert(cur_focus != NULL); //should not have called this function if cur is null

    int depth = 0;
    while(cur_focus->parent != NULL){
        cur_focus = cur_focus->parent;
        depth++;
    }
    return depth;
}

//Returns sibling (makes life way easier)
struct LogicAgent* logic_remove_leaf(struct LogicMaster* ws, struct LogicAgent* to_delete){
    assert(to_delete->left == NULL && to_delete->right == NULL); //should only remove leaf nodes

    if(to_delete == ws->root){
        ws->root = NULL;
        free(to_delete);
        return NULL;
    }

    struct LogicAgent* delete_parent = to_delete->parent;
    int split_dir = delete_parent->split_dir;  
    int size_to_distribute = to_delete->end_cord[split_dir] - to_delete->start_cord[split_dir];
    struct LogicAgent* sibling = delete_parent->left == to_delete ? delete_parent->right : delete_parent->left;
    int shift = get_init_shift(split_dir, to_delete, sibling);
    distribute_space(sibling, size_to_distribute, shift, split_dir); 

    //frees unneeded nodes
    sibling->parent = delete_parent->parent;
    if(delete_parent->parent != NULL){
        if(delete_parent->parent->left == delete_parent){
            delete_parent->parent->left = sibling;
        }else{
            delete_parent->parent->right = sibling;
        }
    }else{
        //removing the ws root 
        ws->root = sibling; 
    }

    if(ws->cur_focus == to_delete){
        ws->cur_focus = NULL;
    }

    free(delete_parent); 
    free(to_delete);
    return sibling;
}

int get_init_shift(int split_dir, struct LogicAgent* to_delete, struct LogicAgent* sibling){
    if(to_delete->start_cord[split_dir] > sibling->start_cord[split_dir]){
        return 0;
    }
    return sibling->start_cord[split_dir] - to_delete->start_cord[split_dir];
}

void distribute_space(struct LogicAgent* root, int size, int shift, int dir){
    if(root == NULL){
        assert(size <= 0); // should not lose size
        return;
    }
    //gives size to root
    root->end_cord[dir] += size;
    //shifts stuff
    root->start_cord[dir] += shift; 
    root->end_cord[dir] += shift; 
    if(root->left == NULL && root->right == NULL){
        return;
    }

    int size_per = size;
    int leftovers = 0;
    int shift_per = shift;
    if(root->split_dir == dir){
        size_per = size/2; 
        leftovers = size%2;
        shift_per = shift/2;
    }
    distribute_space(root->left, size_per, shift_per, dir);
    distribute_space(root->right, size_per+leftovers, shift_per, dir);
}

/* -- updating focus frame -- */
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
    assert(cur != NULL); //there are no nodes -> how did we get here 

    while(cur->right != NULL || cur->left != NULL){
        if(cur->right != NULL){
            cur = cur->right;
        }else{
            cur = cur->left;
        }
    }
    return cur;
}
