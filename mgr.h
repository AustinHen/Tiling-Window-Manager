#include <X11/Xlib.h>
#include <stdlib.h>
 
//a workspace is just a glorified root
struct WorkSpace{
    Window root;
    struct sizes* sizesX; // array of window sizes for x
    struct sizes* sizesY; // window sizes for y
    int width; // # windows across
    int height; // # windows up/down
    int numWindows; // # windows
    struct WindowFrame* windows; //linked list of windows 
};

//handler for windows
struct WindowFrame{
    Window frame;
    struct WindowFrame* next; //so it can be a linked list
    //denotes the indexes in the workspace grid where the window lies
    int rangeX[2]; //rangeX[0] < rangeX[1]
    int rangeY[2]; 
};

//linked list for the sizes
struct sizes{
    int val;
    struct sizes* next;
};

//mgr.c
int init_mgr(Display** display, Window* root);
void mgr_event_loop();
void mgr_error_handler();
void handleConfigureRequest(Display* display, Window root, XConfigureRequestEvent event);
void handleMapRequest(Display* display, Window root, XMapRequestEvent event);

//workSpaceUtils.c
void init_workspace(struct WorkSpace* workspace, Display* display, Window root);
void frame_window(struct WindowFrame* frame, struct WorkSpace* workspace, Window to_add, Display* display);
int get_window_size_px(struct sizes* s, int indexs[2]);
int get_index_sizes(struct sizes* cur, int index);
void split_node_indexes_adj(struct WindowFrame* head, struct WindowFrame* doNotUpdate, int index, int direction);
void split_cur_node(int splitRange[2], int splitDir, struct sizes* sizesToUpdate, struct WorkSpace* workspace, struct WindowFrame* cur_frame, struct WindowFrame* to_add_frame);
