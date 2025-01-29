#include <X11/Xlib.h>
#include <stdlib.h>
 
struct LogicMaster{
    struct LogicAgent* root;
    int min_cord[2];
    int max_cord[2];
};

struct LogicAgent{
    int start_cord[2];
    int end_cord[2];
    int split_dir;
    struct LogicAgent* left;
    struct LogicAgent* right;
    struct LogicAgent* parent;
};

//a workspace is just a glorified root
struct WorkSpace{
    Window root;
    struct LogicMaster* logic_master;
    struct WindowFrame* windows; //linked list of windows 
};

//handler for windows
struct WindowFrame{
    Window frame;
    struct LogicAgent* la;
    struct WindowFrame* next; //so it can be a linked list
};

//linked list for the sizes
struct sizes{
    int val;
    struct sizes* next;
};


void logic_test();

struct LogicAgent* logic_add(struct LogicMaster* ws, struct LogicAgent* cur_focus);
struct LogicAgent* get_default_cur_focus(struct LogicMaster* ws);
void logic_split(struct LogicMaster* ws, struct LogicAgent* to_split, struct LogicAgent* to_add);
int get_split_dir(struct LogicAgent* cur_focus);
int get_depth(struct LogicAgent* cur_focus);
void logic_remove_leaf(struct LogicMaster* ws, struct LogicAgent* to_delete);
void distribute_space(struct LogicAgent* root, int size, int shift, int dir);

//mgr.c
int init_mgr(Display** display, Window* root);
void mgr_event_loop();
void mgr_error_handler();
void handleConfigureRequest(Display* display, Window root, XConfigureRequestEvent event);
void handleMapRequest(Display* display, struct WorkSpace* workspace, XMapRequestEvent event);

//workSpaceUtils.c
void init_workspace(struct WorkSpace* workspace, Display* display, Window root);
void add_first_window(struct WorkSpace* workspace, Display* display, Window to_add);
void frame_window(struct WindowFrame* frame, struct WorkSpace* workspace, Window to_add, Display* display);
int get_window_size_px(struct sizes* s, int indexs[2]);
int get_index_sizes(struct sizes* cur, int index);
void split_node_indexes_adj(struct WindowFrame* head, struct WindowFrame* doNotUpdate, int index, int direction);
void split_cur_node(int splitRange[2], int splitDir, struct sizes* sizesToUpdate, struct WorkSpace* workspace, struct WindowFrame* cur_frame, struct WindowFrame* to_add_frame);
