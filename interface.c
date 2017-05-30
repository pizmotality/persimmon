#include "interface.h"

uint32_t b_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
uint32_t b_em_values = XCB_EVENT_MASK_EXPOSURE |
                       XCB_EVENT_MASK_BUTTON_PRESS |
                       XCB_EVENT_MASK_BUTTON_RELEASE;

uint32_t m_mask = XCB_CW_EVENT_MASK;
uint32_t m_values[] = {
    XCB_EVENT_MASK_EXPOSURE     |
    XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE  |
    XCB_EVENT_MASK_KEY_PRESS    | XCB_EVENT_MASK_KEY_RELEASE
};

xcb_window_t create_window(xcb_connection_t* connection, xcb_screen_t* screen,
    int16_t x, int16_t y, uint16_t width, uint16_t height) {
    xcb_window_t window = xcb_generate_id(connection);
    xcb_create_window(
        connection,
        XCB_COPY_FROM_PARENT,
        window, screen->root,
        x, y, width, height, 0,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        screen->root_visual,
        m_mask, m_values
    );
    xcb_map_window(connection, window);

    return window;
}

xcb_window_t create_subwindow(xcb_connection_t* connection, xcb_window_t parent,
    xcb_screen_t* screen, int16_t x, int16_t y, uint16_t width, uint16_t height,
    uint32_t mask, uint32_t em_values, uint32_t colour) {
    xcb_window_t subwindow = xcb_generate_id(connection);

    uint32_t values[] = {colour, em_values};
    xcb_create_window(
        connection,
        XCB_COPY_FROM_PARENT,
        subwindow, parent,
        x, y, width, height, 0,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        screen->root_visual,
        mask, values
    );

    return subwindow;
}

xcb_window_t create_button(xcb_connection_t* connection, xcb_window_t parent,
    xcb_screen_t* screen, int16_t x, int16_t y, uint16_t width, uint16_t height,
    uint32_t colour) {
    return create_subwindow(
        connection, parent, screen, x, y, width, height,
        b_mask, b_em_values, colour
    );
}
