#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>
#include <stdint.h>

struct Window;
typedef struct Window Window;

typedef struct
{
  char *title;
  int width;
  int height;
  int max_fps;
  bool show_fps;
} WindowConfig;

typedef struct
{
  bool quit;
  bool key_pressed;
  int frame_count;
  float fps;
  uint32_t last_time;
} WindowState;

Window *create_window(char *title, int width, int height);
void get_window_size_in_pixels(Window *wind, int *w, int *h);
void free_window(Window *win);

#endif
