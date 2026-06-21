#include "window.h"

#include <SDL3/SDL.h>
#include <stdio.h>

Window *create_window(char *title, int width, int height)
{
  SDL_Window *win = SDL_CreateWindow(title,
                                     width, height,
                                     SDL_WINDOW_RESIZABLE);

  if (win == NULL)
  {
    fprintf(stderr, "Window could not be created! SDL_Error: %s\n",
            SDL_GetError());
    return NULL;
  }

  return (Window *)win;
}

// static void update_frame_counter(WindowCtx *ctx)
// {
//   ctx->state.frame_count++;

//   uint32_t current_time = SDL_GetTicks();
//   uint32_t time_elapsed = current_time - ctx->state.last_time; // In milliseconds

//   if (time_elapsed >= 1000)
//   {
//     ctx->state.fps =
//         (float)ctx->state.frame_count / (time_elapsed / 1000.0f);

//     ctx->state.frame_count = 0;
//     ctx->state.last_time = current_time;

//     char formatted_title[256];
//     snprintf(formatted_title, sizeof(formatted_title), "%s - FPS: %.2f", ctx->window_config->title,
//              ctx->state.fps);

//     SDL_SetWindowTitle(ctx->window, formatted_title);
//   }
// }

void get_window_size_in_pixels(Window *win, int *w, int *h)
{
  SDL_GetWindowSizeInPixels((SDL_Window *)win, w, h);
}

void free_window(Window *win)
{
  if (win)
    SDL_DestroyWindow((SDL_Window *)win);
}
