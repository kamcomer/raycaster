#ifndef SDL_INPUT_INT_H
#define SDL_INPUT_INT_H

#include "internal/input/input_int.h"
#include "raycaster/input.h"
#include <SDL3/SDL.h>

typedef struct {
  const uint8_t *keyboard_state;
  uint8_t prev_keyboard_state[SDL_SCANCODE_COUNT];
  bool quit_requested;
} SdlInputData;

RcInput *rc_sdl_input_create(void);

extern RcInputVtbl sdl_input_vtbl;
#endif
