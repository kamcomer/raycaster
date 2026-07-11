#include "SDL3/SDL_mouse.h"
#include "alpha.h"
#include "internal/platform/sdl/sdl_input_int.h"

#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdlib.h>

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

static void a_sdl_input_data_destroy(SdlInputData *data)
{
  if (!data) {
    return;
  }
  free(data);
}

static AResult a_sdl_input_data_init(SdlInputData *out)
{
  if (!out) {
    return A_RES_INVLD_ARG;
  }

  *out = (SdlInputData){0};

  out->keyboard_state = SDL_GetKeyboardState(NULL);
  return A_RES_OK;
}

static AResult a_sdl_input_data_create(SdlInputData **out)
{
  if (!out) {
    return A_RES_INVLD_ARG;
  }

  *out = NULL;

  SdlInputData *data = malloc(sizeof(SdlInputData));
  if (!data) {
    return A_RES_ALLOC_ERR;
  }

  AResult res = a_sdl_input_data_init(data);
  if (res != A_RES_OK) {
    return res;
  }

  *out = data;
  return A_RES_OK;
}

static AResult a_sdl_input_init(AInput *out)
{
  if (!out) {
    return A_RES_INVLD_ARG;
  }

  *out = (AInput){0};

  AResult res = a_sdl_input_data_create(&out->impl);
  if (res != A_RES_OK) {
    *out = (AInput){0};
    return res;
  }
  out->vtbl = &sdl_input_vtbl;
  return A_RES_OK;
}

AResult a_sdl_input_create(AInput **out)
{
  if (!out) {
    return A_RES_INVLD_ARG;
  }

  *out = NULL;

  AInput *input = malloc(sizeof(AInput));
  if (!input) {
    return A_RES_ALLOC_ERR;
  }

  AResult res = a_sdl_input_init(input);
  if (res != A_RES_OK) {
    return res;
  }

  *out = input;
  return A_RES_OK;
}

static void a_sdl_input_destroy(AInput *input)
{
  a_sdl_input_data_destroy(input->impl);
  free(input);
}

static void a_sdl_input_update_keyboard(SdlInputData *out)
{
  if (!out || !out->keyboard_state) {
    return;
  }

  for (int i = 0; i < SDL_SCANCODE_COUNT; i++) {
    out->prev_keyboard_state[i] = out->keyboard_state[i];
  }
}

static void a_sdl_input_update_mouse(SdlInputData *out)
{
  if (!out || !out->keyboard_state) {
    return;
  }

  out->mouse_button_state = SDL_GetRelativeMouseState(&out->mouse_dx, &out->mouse_dy);
}

static void a_sdl_input_update(AInput *input)
{
  SdlInputData *data = input->impl;
  a_sdl_input_update_keyboard(data);
}

static bool a_sdl_input_get_key_down(AInput *input, AKey key)
{
  SdlInputData *data = input->impl;
  if (!data || !data->keyboard_state) {
    return false;
  }

  SDL_Scancode scan_code = key_to_scancode(key);
  return data->keyboard_state[scan_code] != 0;
}

static bool a_sdl_input_get_key_pressed(AInput *input, AKey key)
{
  SdlInputData *data = input->impl;
  if (!data || !data->keyboard_state) {
    return false;
  }

  SDL_Scancode scan_code = key_to_scancode(key);
  return data->keyboard_state[scan_code] && !data->prev_keyboard_state[scan_code];
}

AInputVtbl sdl_input_vtbl = {
    .down = a_sdl_input_get_key_down,
    .pressed = a_sdl_input_get_key_pressed,
    .update = a_sdl_input_update,
    .destroy = a_sdl_input_destroy,
};
