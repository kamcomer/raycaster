#include "internal/platform/sdl/sdl_input_int.h"

#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdlib.h>

// static SDL_Keycode key_to_sdl(RcKey key)
// {
//   switch (key) {
//   case RC_KEY_W:
//     return SDLK_W;
//   case RC_KEY_S:
//     return SDLK_S;
//   case RC_KEY_A:
//     return SDLK_A;
//   case RC_KEY_D:
//     return SDLK_D;
//   case RC_KEY_UP:
//     return SDLK_UP;
//   case RC_KEY_DOWN:
//     return SDLK_DOWN;
//   case RC_KEY_LEFT:
//     return SDLK_LEFT;
//   case RC_KEY_RIGHT:
//     return SDLK_RIGHT;
//   case RC_KEY_SPACE:
//     return SDLK_SPACE;
//   case RC_KEY_ESCAPE:
//     return SDLK_ESCAPE;
//   case RC_KEY_ENTER:
//     return SDLK_RETURN;
//   case RC_KEY_SHIFT:
//     return SDLK_LSHIFT;
//   case RC_KEY_CTRL:
//     return SDLK_LCTRL;
//   case RC_KEY_0:
//     return SDLK_0;
//   case RC_KEY_1:
//     return SDLK_1;
//   case RC_KEY_2:
//     return SDLK_2;
//   case RC_KEY_3:
//     return SDLK_3;
//   case RC_KEY_4:
//     return SDLK_4;
//   case RC_KEY_5:
//     return SDLK_5;
//   case RC_KEY_6:
//     return SDLK_6;
//   case RC_KEY_7:
//     return SDLK_7;
//   case RC_KEY_8:
//     return SDLK_8;
//   case RC_KEY_9:
//     return SDLK_9;
//   default:
//     return SDLK_UNKNOWN;
//   }
// }

static SDL_Scancode key_to_scancode(AKey key)
{
  switch (key) {
  case A_KEY_W:
    return SDL_SCANCODE_W;
  case A_KEY_S:
    return SDL_SCANCODE_S;
  case A_KEY_A:
    return SDL_SCANCODE_A;
  case A_KEY_D:
    return SDL_SCANCODE_D;
  case A_KEY_UP:
    return SDL_SCANCODE_UP;
  case A_KEY_DOWN:
    return SDL_SCANCODE_DOWN;
  case A_KEY_LEFT:
    return SDL_SCANCODE_LEFT;
  case A_KEY_RIGHT:
    return SDL_SCANCODE_RIGHT;
  case A_KEY_SPACE:
    return SDL_SCANCODE_SPACE;
  case A_KEY_ESCAPE:
    return SDL_SCANCODE_ESCAPE;
  case A_KEY_ENTER:
    return SDL_SCANCODE_RETURN;
  case A_KEY_SHIFT:
    return SDL_SCANCODE_LSHIFT;
  case A_KEY_CTRL:
    return SDL_SCANCODE_LCTRL;
  case A_KEY_0:
    return SDL_SCANCODE_0;
  case A_KEY_1:
    return SDL_SCANCODE_1;
  case A_KEY_2:
    return SDL_SCANCODE_2;
  case A_KEY_3:
    return SDL_SCANCODE_3;
  case A_KEY_4:
    return SDL_SCANCODE_4;
  case A_KEY_5:
    return SDL_SCANCODE_5;
  case A_KEY_6:
    return SDL_SCANCODE_6;
  case A_KEY_7:
    return SDL_SCANCODE_7;
  case A_KEY_8:
    return SDL_SCANCODE_8;
  case A_KEY_9:
    return SDL_SCANCODE_9;
  default:
    return SDL_SCANCODE_UNKNOWN;
  }
}

static void destroy_data(SdlInputData *data)
{
  if (!data) {
    return;
  }
  free(data);
}

static void update_data(SdlInputData *data)
{
  if (!data || !data->keyboard_state) {
    return;
  }

  for (int i = 0; i < SDL_SCANCODE_COUNT; i++) {
    data->prev_keyboard_state[i] = data->keyboard_state[i];
  }

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      data->quit_requested = true;
    }
  }
}

static bool get_input_down(SdlInputData *data, AKey key)
{
  if (!data || !data->keyboard_state) {
    return false;
  }

  SDL_Scancode scan_code = key_to_scancode(key);
  return data->keyboard_state[scan_code] != 0;
}

static bool get_input_presssed(SdlInputData *data, AKey key)
{
  if (!data || !data->keyboard_state) {
    return false;
  }

  SDL_Scancode scan_code = key_to_scancode(key);
  return data->keyboard_state[scan_code] && !data->prev_keyboard_state[scan_code];
}

AInput *a_sdl_input_create(void)
{
  SdlInputData *data = calloc(1, sizeof(SdlInputData));
  if (!data) {
    return NULL;
  }

  data->keyboard_state = SDL_GetKeyboardState(NULL);
  data->quit_requested = false;

  AInput *input = malloc(sizeof(AInput));
  if (!input) {
    destroy_data(data);
    return NULL;
  }
  input->vtbl = &sdl_input_vtbl;
  input->impl = data;
  return input;
}

static bool sdl_input_down(AInput *input, AKey key) { return get_input_down(input->impl, key); }

static bool sdl_input_pressed(AInput *input, AKey key)
{
  return get_input_presssed(input->impl, key);
}

static void sdl_input_update(AInput *input) { update_data(input->impl); }

static void sdl_input_destroy(AInput *input)
{
  destroy_data(input->impl);
  free(input);
}

AInputVtbl sdl_input_vtbl = {
    .down = sdl_input_down,
    .pressed = sdl_input_pressed,
    .update = sdl_input_update,
    .destroy = sdl_input_destroy,
};
