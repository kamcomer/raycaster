#ifndef _WINDOW_EVENT_H
#define _WINDOW_EVENT_H

#include "window.h"

typedef enum
{
    WINDOW_EVENT_NONE,
    WINDOW_EVENT_RESIZED,
    WINDOW_EVENT_CLOSE,
    WINDOW_EVENT_SHOWN,
    WINDOW_EVENT_HIDDEN,
    WINDOW_EVENT_FOCUS_GAINED,
    WINDOW_EVENT_FOCUS_LOST,
    WINDOW_EVENT_ENTER,
    WINDOW_EVENT_LEAVE,
    WINDOW_EVENT_MINIMIZED,
    WINDOW_EVENT_MAXIMIZED,
    WINDOW_EVENT_RESTORED,
} WindowEventId;

typedef struct
{
    WindowEventId type;
    int data1;
    int data2;
} WindowEvent;

bool poll_window_event(Window *win, WindowEvent *event_out);

#endif