#include "../include/mgr.h"
#include <X11/Xlib.h>
#define NAV_BAR_COLOR 0x1f1e2f 
#define NAV_BAR_BORDER_COLOR 0x778ca8 
#define DEFAULT_TEXT_COLOR 0xffffff
#include <stdio.h>
#include <string.h>


//sets up the bar 
void create_nav_bar(struct NavBarState* state, Display* display, Window root){
    XWindowAttributes screenAttributes; 
    XGetWindowAttributes(display, root, &screenAttributes);

    state->w = XCreateSimpleWindow(
            display,
            root,
            0, 
            0,
            screenAttributes.width, 
            23,
            0,
            NAV_BAR_BORDER_COLOR,
            NAV_BAR_COLOR
            );

    XMapWindow(display, state->w); 

    XRaiseWindow(display, state->w);

    state->cur_ws = 1;

    update_nav_bar(state, display);
}

void update_nav_bar(struct NavBarState* state, Display* display) {
    XClearWindow(display, state->w);

    GC gc = XCreateGC(display, state->w, 0, NULL);
    XFontStruct* font = XLoadQueryFont(display, "fixed");
    if (font) {
        XSetFont(display, gc, font->fid);
    }

    int offset = 5;
    char to_print[128] = "";  

    for (int i = 1; i < 11; i++) {
        int true_index = (i == 10) ? 0 : i;

        if (state->cur_ws == true_index) {
            offset = draw_nav_str(to_print, offset, display, state->w, gc, font, 0);
            snprintf(to_print, sizeof(to_print), " %d ", true_index);
            offset = draw_nav_str(to_print, offset, display, state->w, gc, font, 1);
            to_print[0] = '\0';
        } else {
            if (state->ws_counts[true_index] < 1) continue;

            char append[8];
            snprintf(append, sizeof(append), " %d ", true_index);

            if (strlen(to_print) + strlen(append) < sizeof(to_print)) {
                strcat(to_print, append);
            }
        }
    }

    offset = draw_nav_str(to_print, offset, display, state->w, gc, font, 0);

    // cleanup
    if (font) XFreeFont(display, font);
    XFreeGC(display, gc);
}

int draw_nav_str(const char* to_draw, int offset, Display* display, Window nav_win, GC gc, XFontStruct* font, int is_cur_win) {
    if (!font || !to_draw || strlen(to_draw) == 0) return offset;

    int y = font->ascent + 4; 

    XSetForeground(display, gc, is_cur_win ? DEFAULT_TEXT_COLOR : NAV_BAR_BORDER_COLOR);
    XDrawString(display, nav_win, gc, offset, y, to_draw, strlen(to_draw));

    return offset + XTextWidth(font, to_draw, strlen(to_draw));
}
