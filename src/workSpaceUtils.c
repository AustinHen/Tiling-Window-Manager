#include "../include/mgr.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h> 
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

#define DEFAULT_WINDOW_COLOR 0xF48FB1
#define DEFAULT_FRAME_COLOR 0xFFFFFF
//sets up workspace 
void init_workspace(struct WorkSpace* workspace, int workspace_num, Display* display, Window root){
    //grabs the screen's atributes
    XWindowAttributes screenAttributes; 
    XGetWindowAttributes(display, root, &screenAttributes);
    
    const int padding = 10;
    struct LogicMaster* lm = (struct LogicMaster*) malloc(sizeof(struct LogicMaster));
    lm->root = NULL;
    lm->min_cord[0] = padding;
    lm->min_cord[1] = padding;
    lm->max_cord[0] = screenAttributes.width - padding;
    lm->max_cord[1] = screenAttributes.height - padding;
    lm->cur_focus = NULL;

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
            DEFAULT_WINDOW_COLOR
    );

    XSelectInput( display, workspace->root, SubstructureRedirectMask | SubstructureNotifyMask);

    init_workspace_background(workspace_num, display, workspace->root, root);

}

void init_workspace_background(int workspace_num, Display* display, Window window, Window root){
    return; //TODO finish function
    int img_width, img_height, channels;

    if(workspace_num > 9 || workspace_num < 0){
        return; //just so no buffer overflow stuff 
    }
    char primary_file_name[16];
    sprintf(primary_file_name, "./images/%d.png", workspace_num); 
    
    unsigned char* img_data = stbi_load(primary_file_name, &img_width, &img_height, &channels, 4); 

    //failed primary try secondary
    if (!img_data) {
        printf("failed to load primary");

        char* secondary_file_name = "./images/default.png";
        img_data = stbi_load(secondary_file_name, &img_width, &img_height, &channels, 4); 
    }

    //failed both just return
    if(!img_data){
        printf("failed to load secondary");
        return;
    }

    //actually update the window
    int screen = DefaultScreen(display);
    uint32_t* image32 = malloc(img_width * img_height * sizeof(uint32_t));
    for (int i = 0; i < img_width * img_height; ++i) {
        unsigned char* p = &img_data[i * 4];
        image32[i] = (p[0] << 16) | (p[1] << 8) | (p[2]); // RGB (no alpha)
    }

    // Create pixmap and draw image onto it
    Pixmap pixmap = XCreatePixmap(display, root, img_width, img_height, DefaultDepth(display, screen));
    GC gc = XCreateGC(display, pixmap, 0, NULL);

    XImage* ximage = XCreateImage(
            display,
            DefaultVisual(display, screen),
            DefaultDepth(display, screen),
            ZPixmap, 0,
            (char*)image32,
            img_width, img_height,
            32, 0
    );

    XPutImage(display, pixmap, gc, ximage, 0, 0, 0, 0, img_width, img_height);

    XSetWindowBackgroundPixmap(display, window, pixmap);


    stbi_image_free(img_data);
    XDestroyImage(ximage); 
    XFreeGC(display, gc);
    XFreePixmap(display, pixmap);

}

void add_window_to_workspace(struct WorkSpace* workspace, Display* display, Window to_add, struct WindowFrame* cur_frame){
    struct WindowFrame* to_add_frame = (struct WindowFrame*) malloc(sizeof(struct WindowFrame));
    to_add_frame->w = to_add;

    struct LogicAgent* cur_frame_la = cur_frame == NULL ? NULL : cur_frame->la;

    to_add_frame->la = logic_add(workspace->logic_master, cur_frame_la);
    to_add_frame->la->window_frame = to_add_frame;
    frame_window(to_add_frame, workspace, to_add, display);

    update_all_children_frames(to_add_frame->la->parent, display);
    
    //update focus
    workspace->logic_master->cur_focus = to_add_frame->la;
} 

void frame_window(struct WindowFrame* frame, struct WorkSpace* workspace, Window to_add, Display* display){
    int x = frame->la->start_cord[0] + PADDING;
    int y = frame->la->start_cord[1] + PADDING;
    int width = frame->la->end_cord[0] - frame->la->start_cord[0] - (2*PADDING); 
    int height = frame->la->end_cord[1] - frame->la->start_cord[1] - (2*PADDING); 

    frame->frame = XCreateSimpleWindow(
            display,
            workspace->root,
            x,
            y,
            width,
            height,
            0,
            0, //border color
            get_random_color());

    XSelectInput(
            display,
            frame->frame,
            SubstructureRedirectMask | SubstructureNotifyMask);
  
    //puts the window in the frame 
    XReparentWindow( display, to_add, frame->frame, 0, 0);  
    XMapWindow(display, to_add); //NOTE this will not display until the parent is displayed
    XMapWindow(display, frame->frame); //NOTE this will not display until the parent is displayed

    XWindowChanges changes;
    changes.x = 0;
    changes.y = 0;
    changes.height = height;
    changes.width =  width;
    changes.border_width = 0;
    unsigned int val_mask = CWWidth  | CWHeight | CWX | CWY | CWBorderWidth;
    XConfigureWindow(display, to_add, val_mask, &changes); 
}

void update_all_children_frames(struct LogicAgent* root, Display* display_){
    if(root == NULL){
        return;
    }
    if(root->window_frame != NULL){
        update_frame(root->window_frame, display_);
    }

    update_all_children_frames(root->left, display_);
    update_all_children_frames(root->right, display_);
}

//updates a frame to match new logic representation
void update_frame(struct WindowFrame* to_update, Display* display_){
    struct LogicAgent* la = to_update->la;
    XWindowChanges changes;
    changes.x = la->start_cord[0] + PADDING;
    changes.y = la->start_cord[1] + PADDING;
    changes.height = la->end_cord[1] - la->start_cord[1] - (2*PADDING);
    changes.width =  la->end_cord[0] - la->start_cord[0] - (2*PADDING);
    changes.border_width = 0;
    unsigned int val_mask = CWWidth  | CWHeight | CWX | CWY | CWBorderWidth;
    XConfigureWindow(display_, to_update->frame, val_mask, &changes); 

    //re-use changes
    changes.x = 0;
    changes.y = 0;
    XConfigureWindow(display_, to_update->w, val_mask, &changes); 
}



struct LogicAgent* find_windows_la(struct LogicAgent* root, Window to_find){
    if(root == NULL){
        return NULL;
    } 

    if(root->window_frame != NULL && root->window_frame->w == to_find){
        return root;
    }

    struct LogicAgent* left = find_windows_la(root->left, to_find);
    if(left != NULL) {return left;}
    struct LogicAgent* right = find_windows_la(root->right, to_find);
    if(right != NULL) {return right;}

    return NULL;
}

int get_random_color(){
    //0 - 16777215 is the range of colors 
    //int cool_colors[] = {0x2596BE, 0xeab676, 0x873e23, 0xabdbe3};
    //TODO add seed 
    return rand() % (16777215); 
}
