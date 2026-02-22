#include "event.h"
#include "player.h"
#include "scene.h"

#include <stdio.h>
#include <stdlib.h>

void handle_window_events(Window *win)
{
    WindowEvent event;
    while (poll_window_event(win, &event))
    {
        if (event.type != WINDOW_EVENT_NONE)
        {
            handle_window_event(&event);
        }
    }
}

void handle_window_event(WindowEvent *event)
{
    switch (event->type)
    {
    case WINDOW_EVENT_RESIZED:
        printf("Window resized to %d x %d\n", event->data1, event->data2);
        break;
    case WINDOW_EVENT_CLOSE:
        printf("Window close requested\n");
        exit(1);
        break;
    case WINDOW_EVENT_SHOWN:
        printf("Window shown\n");
        break;
    case WINDOW_EVENT_HIDDEN:
        printf("Window hidden\n");
        break;
    case WINDOW_EVENT_FOCUS_GAINED:
        printf("Window focus gained\n");
        break;
    case WINDOW_EVENT_FOCUS_LOST:
        printf("Window focus lost\n");
        break;
    case WINDOW_EVENT_ENTER:
        printf("Mouse entered window\n");
        break;
    case WINDOW_EVENT_LEAVE:
        printf("Mouse left window\n");
        break;
    case WINDOW_EVENT_MINIMIZED:
        printf("Window minimized\n");
        break;
    case WINDOW_EVENT_MAXIMIZED:
        printf("Window maximized\n");
        break;
    case WINDOW_EVENT_RESTORED:
        printf("Window restored\n");
        break;
    default:
        break;
    }
}