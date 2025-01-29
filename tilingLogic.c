#include "mgr.h"
#include <stdio.h>

struct LogicAgent* logic_add(struct LogicMaster* ws, struct LogicAgent* cur_focus){
    //sets up the basic format of the new agent
    struct LogicAgent* to_add = (struct LogicAgent*) malloc(sizeof(struct LogicAgent));
    to_add->left = NULL;
    to_add->right = NULL;
    to_add->parent = NULL;

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
    return to_add; 
}

//returns a valid cur_focus
struct LogicAgent* get_default_cur_focus(struct LogicMaster* ws){
    struct LogicAgent* cur = ws->root;
    if(cur == NULL){
        //there are no nodes -> how did we get here 
        exit(2);
    } 

    while(cur->right != NULL || cur->left != NULL){
        if(cur->right != NULL){
            cur = cur->right;
        }else{
            cur = cur->left;
        }
    }
    return cur;
}

//splits to_split, giving half its space to to_add and reparents both nodes
void logic_split(struct LogicMaster* ws, struct LogicAgent* to_split, struct LogicAgent* to_add){
    if(to_split == NULL || to_split->right != NULL || to_split->left != NULL){
        //to split should be a leaf node
        exit(1);
    }

    int split_dir = get_split_dir(to_split);
    int non_split_dir = (split_dir + 1) % 2;

    //makes the parent of to_add and to_split; yes there is a lot of copying 
    struct LogicAgent* new_parent = (struct LogicAgent*) malloc(sizeof(struct LogicAgent));
    new_parent->parent = to_split->parent;
    new_parent->split_dir = split_dir;
    if(new_parent->parent != NULL){
        if(new_parent->parent->left == to_split){
            new_parent->parent->left = new_parent;
        }else{
            new_parent->parent->right = new_parent;
        }
    }
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
    if(depth == -1){
        //should find cur_focus
        exit(2);
    }

    return depth % 2;
}

int get_depth(struct LogicAgent* cur_focus){
    //just go up till parrent = null
    if(cur_focus == NULL){
        //should not have called this function if cur is null
        exit(2);
    }

    int depth = 0;
    while(cur_focus->parent != NULL){
        cur_focus = cur_focus->parent;
        depth++;
    }
    return depth;
}

void logic_remove_leaf(struct LogicMaster* ws, struct LogicAgent* to_delete){
    if(to_delete->left != NULL || to_delete-> right != NULL){
        //should only remove leaf nodes
        exit(2);
    }

    if(to_delete == ws->root){
        ws->root = NULL;
        free(to_delete);
        return;
    }

    struct LogicAgent* delete_parent = to_delete->parent;
    int split_dir = delete_parent->split_dir;  
    int size_to_distribute = to_delete->end_cord[split_dir] - to_delete->start_cord[split_dir];
    struct LogicAgent* sibling = delete_parent->left == to_delete ? delete_parent->right : delete_parent->left;
    distribute_space(sibling, to_delete, size_to_distribute, split_dir); 

    //frees unneeded nodes
    sibling->parent = delete_parent->parent;
    if(delete_parent->parent != NULL){
        if(delete_parent->parent->left == delete_parent){
            delete_parent->parent->left = sibling;
        }else{
            delete_parent->parent->right = sibling;
        }
    }
    free(delete_parent); 
    free(to_delete);
}

void distribute_space(struct LogicAgent* root, int size, int shift, int dir){
    if(root == NULL){
        if(size > 0){
            //how did we get here
            exit(0); 
        }
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

