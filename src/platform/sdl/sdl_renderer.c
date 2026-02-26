#include "renderer.h"

#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>

Renderer *create_renderer(Window *win)
{
    SDL_Renderer *rend = SDL_CreateRenderer((SDL_Window *)win, NULL);

    if (rend == NULL)
    {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n",
                SDL_GetError());
        SDL_DestroyWindow((SDL_Window *)win);
        return NULL;
    }

    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rend, 0xFF, 0xFF, 0xFF, 0xFF);

    return (Renderer *)rend;
}

void render_present(Renderer *rend)
{
    SDL_RenderPresent((SDL_Renderer *)rend);
}

void set_render_draw_color(Renderer *rend, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    SDL_SetRenderDrawColor((SDL_Renderer *)rend, r, g, b, a);
}

void clear_renderer(Renderer *rend)
{
    SDL_RenderClear((SDL_Renderer *)rend);
}

void render_fill_rect(Renderer *rend, Rect *rect)
{
    SDL_FRect frect = {(float)rect->x, (float)rect->y, (float)rect->w, (float)rect->h};
    SDL_RenderFillRect((SDL_Renderer *)rend, &frect);
}

void render_draw_line(Renderer *rend, int x1, int y1, int x2, int y2)
{
    SDL_RenderLine((SDL_Renderer *)rend, (float)x1, (float)y1, (float)x2, (float)y2);
}

Texture *create_texture(Renderer *rend, uint32_t format, uint8_t access_type,
                        int width, int height)
{
    SDL_Texture *texture = SDL_CreateTexture(
        (SDL_Renderer *)rend, format, access_type,
        width, height);
    return (Texture *)texture;
}

void set_texture_blend_mode(Texture *tex, int mode)
{
    SDL_SetTextureBlendMode((SDL_Texture *)tex, mode);
}

void lock_texture(Texture *tex, Rect *rect, void **pixels, int *pitch)
{
    (void)rect;
    SDL_LockTexture((SDL_Texture *)tex, NULL, pixels, pitch);
}
void unlock_texture(Texture *tex)
{
    SDL_UnlockTexture((SDL_Texture *)tex);
}
void destroy_texture(Texture *tex)
{
    SDL_DestroyTexture((SDL_Texture *)tex);
}

void render_copy(Renderer *rend, Texture *tex, Rect *rect1, Rect *rect2)
{
    SDL_FRect *src = NULL;
    SDL_FRect *dst = NULL;
    SDL_FRect src_rect;
    SDL_FRect dst_rect;
    
    if (rect1) {
        src_rect.x = (float)rect1->x;
        src_rect.y = (float)rect1->y;
        src_rect.w = (float)rect1->w;
        src_rect.h = (float)rect1->h;
        src = &src_rect;
    }
    if (rect2) {
        dst_rect.x = (float)rect2->x;
        dst_rect.y = (float)rect2->y;
        dst_rect.w = (float)rect2->w;
        dst_rect.h = (float)rect2->h;
        dst = &dst_rect;
    }
    SDL_RenderTexture((SDL_Renderer *)rend, (SDL_Texture *)tex, src, dst);
}

void free_renderer(Renderer *rend)
{
    SDL_DestroyRenderer((SDL_Renderer *)rend);
    free(rend);
}
