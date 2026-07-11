#ifndef SDL_INPUT_INT_H
#define SDL_INPUT_INT_H

#include "SDL3/SDL_mouse.h"
#include "internal/i_alpha.h"
#include <SDL3/SDL.h>

typedef struct {
  const bool *keyboard_state;
  uint8_t prev_keyboard_state[SDL_SCANCODE_COUNT];

  float mouse_dx;
  float mouse_dy;
  SDL_MouseButtonFlags mouse_button_state;

} SdlInputData;

AResult a_sdl_input_create(AInput **out);

extern AInputVtbl sdl_input_vtbl;
#endif
