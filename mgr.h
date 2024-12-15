#include <X11/Xlib.h>
#include <stdlib.h>
 

int init_mgr(Display** display, Window* root);
void mgr_event_loop();
void mgr_error_handler();
