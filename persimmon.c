#include <stdlib.h>
#include <stdio.h>

#include "state.h"
#include "interface.h"
#include "gtp.h"

int main(int argc, char* argv[]) {
    struct arguments arguments = {19, 0, 0, {0}};

    init_state(&arguments, argc, argv);

    // setup windows
    xcb_connection_t* connection = xcb_connect(NULL, NULL);
    xcb_screen_t* screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

    xcb_window_t m_window = create_window(
        connection, screen, 0, 0, 480, 384);

    xcb_window_t r_window = create_button(
        connection, m_window, screen, 0, 0, 96, 96, 0x8fadff);
    xcb_window_t p_window = create_button(
        connection, m_window, screen, 0, 96, 96, 96, 0xadff8f);
    xcb_window_t t_window = create_button(
        connection, m_window, screen, 0, 192, 96, 96, 0xffda8f);
    xcb_window_t q_window = create_button(
        connection, m_window, screen, 0, 288, 96, 96, 0xff8f8f);
    xcb_map_subwindows(connection, m_window);

    xcb_flush(connection);

    // handle input events
    xcb_generic_event_t* event;
    while ((event = xcb_wait_for_event(connection))) {
        switch (event->response_type & ~0x80) {
            case XCB_EXPOSE: ;
                xcb_expose_event_t* expose = (xcb_expose_event_t*)event;
                printf("expose window %"PRIu32", at (%"PRIu16", %"PRIu16"),"
                       " dimensions: (%"PRIu16", %"PRIu16")\n",
                    expose->window, expose->x, expose->y,
                    expose->width, expose->height);
                break;
            case XCB_BUTTON_PRESS: ;
                xcb_button_press_event_t* bp = (xcb_button_press_event_t*)event;
                if (bp->detail == 1) {
                    printf("button press in window %"PRIu32","
                           " at (%"PRIi16", %"PRIi16")\n",
                        bp->event, bp->event_x, bp->event_y);

                    if (bp->event == r_window) {
                        reset_engines(arguments.state);
                    } else if (bp->event == p_window) {
                        init_engines(arguments);
                    } else if (bp->event == t_window) {
                        make_move(arguments.state);
                    } else if (bp->event == q_window) {
                        quit_engines(arguments.state);
                        return 0;
                    }
                    break;
                }
                break;
            case XCB_BUTTON_RELEASE: ;
                xcb_button_release_event_t* br = (xcb_button_release_event_t*)event;
                printf("button: %"PRIu8" released in window %"PRIu32","
                       " at (%"PRIi16", %"PRIi16")\n",
                    br->detail, br->event, br->event_x, br->event_y);
                break;
            case XCB_KEY_PRESS: ;
                xcb_key_press_event_t* kp = (xcb_key_press_event_t*)event;
                printf("key: %"PRIu8" press in window %"PRIu32"\n",
                    kp->detail, kp->event);
                break;
            case XCB_KEY_RELEASE: ;
                xcb_key_release_event_t* kr = (xcb_key_release_event_t*)event;
                printf("key: %"PRIu8" release in window %"PRIu32"\n",
                    kr->detail, kr->event);
                break;
            default:
                printf("unknown event: %"PRIu8"\n",
                    event->response_type);
                break;
        }

        free(event);
    }

    xcb_disconnect(connection);

    return 0;
}
