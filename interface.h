#ifndef _INTERFACE_H
#define _INTERFACE_H

#include <inttypes.h>

#include <xcb/xcb.h>

#define _B_WTYPE 0
#define _M_WTYPE 1

xcb_window_t create_window(xcb_connection_t* connection, xcb_screen_t* screen,
    int16_t x, int16_t y, uint16_t width, uint16_t height);

xcb_window_t create_subwindow(xcb_connection_t* connection, xcb_window_t parent,
    xcb_screen_t* screen, int16_t x, int16_t y, uint16_t width, uint16_t height,
    uint32_t mask, uint32_t em_values, uint32_t colour);

xcb_window_t create_button(xcb_connection_t* connection, xcb_window_t parent,
    xcb_screen_t* screen, int16_t x, int16_t y, uint16_t width, uint16_t height,
    uint32_t colour);

#endif
