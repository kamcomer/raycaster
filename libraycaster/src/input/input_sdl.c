#include "raycaster/input.h"

#include <SDL.h>
#include <stdbool.h>
#include <stdlib.h>

struct RcInput
{
    const uint8_t *keyboard_state;
    uint8_t prev_keyboard_state[SDL_NUM_SCANCODES];
    bool quit_requested;
};

static SDL_Keycode rc_key_to_sdl(RcKey key)
{
    switch (key)
    {
    case RC_KEY_W: return SDLK_w;
    case RC_KEY_S: return SDLK_s;
    case RC_KEY_A: return SDLK_a;
    case RC_KEY_D: return SDLK_d;
    case RC_KEY_UP: return SDLK_UP;
    case RC_KEY_DOWN: return SDLK_DOWN;
    case RC_KEY_LEFT: return SDLK_LEFT;
    case RC_KEY_RIGHT: return SDLK_RIGHT;
    case RC_KEY_SPACE: return SDLK_SPACE;
    case RC_KEY_ESCAPE: return SDLK_ESCAPE;
    case RC_KEY_ENTER: return SDLK_RETURN;
    case RC_KEY_SHIFT: return SDLK_LSHIFT;
    case RC_KEY_CTRL: return SDLK_LCTRL;
    case RC_KEY_0: return SDLK_0;
    case RC_KEY_1: return SDLK_1;
    case RC_KEY_2: return SDLK_2;
    case RC_KEY_3: return SDLK_3;
    case RC_KEY_4: return SDLK_4;
    case RC_KEY_5: return SDLK_5;
    case RC_KEY_6: return SDLK_6;
    case RC_KEY_7: return SDLK_7;
    case RC_KEY_8: return SDLK_8;
    case RC_KEY_9: return SDLK_9;
    default: return SDLK_UNKNOWN;
    }
}

static SDL_Scancode rc_key_to_scancode(RcKey key)
{
    switch (key)
    {
    case RC_KEY_W: return SDL_SCANCODE_W;
    case RC_KEY_S: return SDL_SCANCODE_S;
    case RC_KEY_A: return SDL_SCANCODE_A;
    case RC_KEY_D: return SDL_SCANCODE_D;
    case RC_KEY_UP: return SDL_SCANCODE_UP;
    case RC_KEY_DOWN: return SDL_SCANCODE_DOWN;
    case RC_KEY_LEFT: return SDL_SCANCODE_LEFT;
    case RC_KEY_RIGHT: return SDL_SCANCODE_RIGHT;
    case RC_KEY_SPACE: return SDL_SCANCODE_SPACE;
    case RC_KEY_ESCAPE: return SDL_SCANCODE_ESCAPE;
    case RC_KEY_ENTER: return SDL_SCANCODE_RETURN;
    case RC_KEY_SHIFT: return SDL_SCANCODE_LSHIFT;
    case RC_KEY_CTRL: return SDL_SCANCODE_LCTRL;
    case RC_KEY_0: return SDL_SCANCODE_0;
    case RC_KEY_1: return SDL_SCANCODE_1;
    case RC_KEY_2: return SDL_SCANCODE_2;
    case RC_KEY_3: return SDL_SCANCODE_3;
    case RC_KEY_4: return SDL_SCANCODE_4;
    case RC_KEY_5: return SDL_SCANCODE_5;
    case RC_KEY_6: return SDL_SCANCODE_6;
    case RC_KEY_7: return SDL_SCANCODE_7;
    case RC_KEY_8: return SDL_SCANCODE_8;
    case RC_KEY_9: return SDL_SCANCODE_9;
    default: return SDL_SCANCODE_UNKNOWN;
    }
}

RcInput *rc_input_create(void)
{
    RcInput *in = calloc(1, sizeof(RcInput));
    if (!in)
        return NULL;

    in->keyboard_state = SDL_GetKeyboardState(NULL);
    in->quit_requested = false;

    return in;
}

void rc_input_destroy(RcInput *in)
{
    free(in);
}

bool rc_input_down(RcInput *in, RcKey key)
{
    if (!in || !in->keyboard_state)
        return false;

    SDL_Scancode sc = rc_key_to_scancode(key);
    return in->keyboard_state[sc] != 0;
}

bool rc_input_pressed(RcInput *in, RcKey key)
{
    if (!in || !in->keyboard_state)
        return false;

    SDL_Scancode sc = rc_key_to_scancode(key);
    return in->keyboard_state[sc] && !in->prev_keyboard_state[sc];
}

void rc_input_update(RcInput *in)
{
    if (!in || !in->keyboard_state)
        return;

    SDL_Scancode sc;
    for (int i = 0; i < SDL_NUM_SCANCODES; i++)
    {
        in->prev_keyboard_state[i] = in->keyboard_state[i];
    }

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            in->quit_requested = true;
        }
    }
}
