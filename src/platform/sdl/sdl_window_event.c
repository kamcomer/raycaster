#include "window_event.h"
#include <SDL3/SDL.h>

bool poll_window_event(Window *win, WindowEvent *event_out)
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    if (event.type >= SDL_EVENT_WINDOW_SHOWN && event.type <= SDL_EVENT_WINDOW_RESIZED)
    {
      if (event.window.windowID == SDL_GetWindowID((SDL_Window *)win))
      {
        event_out->data1 = event.window.data1;
        event_out->data2 = event.window.data2;

        switch (event.window.type)
        {
        case SDL_EVENT_WINDOW_RESIZED:
          event_out->type = WINDOW_EVENT_RESIZED;
          return true;

        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
          event_out->type = WINDOW_EVENT_CLOSE;
          return true;

        case SDL_EVENT_WINDOW_SHOWN:
          event_out->type = WINDOW_EVENT_SHOWN;
          return true;
        case SDL_EVENT_WINDOW_HIDDEN:
          event_out->type = WINDOW_EVENT_HIDDEN;
          return true;
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
          event_out->type = WINDOW_EVENT_FOCUS_GAINED;
          return true;
        case SDL_EVENT_WINDOW_FOCUS_LOST:
          event_out->type = WINDOW_EVENT_FOCUS_LOST;
          return true;
        case SDL_EVENT_WINDOW_MOUSE_ENTER:
          event_out->type = WINDOW_EVENT_ENTER;
          return true;
        case SDL_EVENT_WINDOW_MOUSE_LEAVE:
          event_out->type = WINDOW_EVENT_LEAVE;
          return true;
        case SDL_EVENT_WINDOW_MINIMIZED:
          event_out->type = WINDOW_EVENT_MINIMIZED;
          return true;
        case SDL_EVENT_WINDOW_MAXIMIZED:
          event_out->type = WINDOW_EVENT_MAXIMIZED;
          return true;
        case SDL_EVENT_WINDOW_RESTORED:
          event_out->type = WINDOW_EVENT_RESTORED;
          return true;
        default:
          break;
        }
      }
    }
  }
  event_out->type = WINDOW_EVENT_NONE;
  return false;
}
