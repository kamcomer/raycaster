#ifndef _WINDOW_CTX_H
#define _WINDOW_CTX_H

#include "window.h"
#include "renderer.h"
#include <stdlib.h>

typedef struct
{
    WindowConfig *window_config;
    Window *window;
    Renderer *renderer;
    WindowState state;
    void (*render_function)(void);
} WindowCtx;

WindowCtx *create_window_ctx(WindowConfig *config);
void free_window_ctx(WindowCtx *ctx);

#endif