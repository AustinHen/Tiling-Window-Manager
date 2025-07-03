#include <X11/Xlib.h>
#include <stdlib.h>

#define PADDING 7 

struct LogicMaster{
    struct LogicAgent* root;
    int min_cord[2];
    int max_cord[2];
    struct LogicAgent* cur_focus;
};

struct LogicAgent{
    int start_cord[2];
    int end_cord[2];
    int split_dir;
    struct WindowFrame* window_frame;
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
    Window w;
    struct LogicAgent* la;
    struct WindowFrame* next; //so it can be a linked list
};


void logic_test();

//TODO fix garbage 
//logic
struct LogicAgent* logic_add(struct LogicMaster* ws, struct LogicAgent* cur_focus);
struct LogicAgent* get_default_cur_focus(struct LogicMaster* ws);
void logic_split(struct LogicMaster* ws, struct LogicAgent* to_split, struct LogicAgent* to_add);
int get_split_dir(struct LogicAgent* cur_focus);
int get_depth(struct LogicAgent* cur_focus);
struct LogicAgent* logic_remove_leaf(struct LogicMaster* ws, struct LogicAgent* to_delete); 
int get_init_shift(int split_dir, struct LogicAgent* to_delete, struct LogicAgent* sibling);
void distribute_space(struct LogicAgent* root, int size, int shift, int dir);
struct LogicAgent* get_focus_frame(int split_dir, int dir, struct LogicAgent* cur_focus, struct LogicMaster* ws);
struct LogicAgent* get_focus_frame_bubble_down(int split_dir, int dir, struct LogicAgent* top);
struct LogicAgent* get_focus_frame_bubble_up(int split_dir, int dir, struct LogicAgent* cur_focus);
struct LogicAgent* find_windows_la(struct LogicAgent* root, Window to_find);

//mgr.c
int init_mgr(Display** display, Window* root);
void mgr_event_loop(Display* display, Window root);
void mgr_error_handler();
void handleConfigureRequest(Display* display, Window root, XConfigureRequestEvent event);
void handleMapRequest(Display* display, struct WorkSpace* workspace, XMapRequestEvent event);
void handleUnmapNotify(Display* display, struct WorkSpace* workspaces, int num_ws, XUnmapEvent event);

//workSpaceUtils.c
void init_workspace(struct WorkSpace* workspace, int workspace_num, Display* display, Window root);
void init_workspace_background(int workspace_num, Display* display, Window window, Window root, int screen_width, int screen_height);
void add_window_to_workspace(struct WorkSpace* workspace, Display* display, Window to_add, struct WindowFrame* cur_frame);
void update_frame(struct WindowFrame* to_update, Display* display_);
void update_all_children_frames(struct LogicAgent* root, Display* display_);
void frame_window(struct WindowFrame* frame, struct WorkSpace* workspace, Window to_add, Display* display);
int get_random_color();
void update_focus(int split_dir, int dir, Display* display_, struct WorkSpace* workspace);

//keybs
void grab_all_keys(Display* display_, Window w);
void open_terminal();
void open_app_launcher();
void handleKeyPress(Display* display, Window root, XKeyEvent event, struct WorkSpace *workspaces, int* cur_focus_idx);
//tests
void logic_test();
