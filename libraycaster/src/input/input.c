#include "internal/input/input_int.h"
#include "internal/platform/sdl/sdl_input_int.h"

RcInput *rc_input_create(RcInputBackend backend)
{
  switch (backend) {
  case RC_INPUT_BACKEND_SDL:
    return rc_sdl_input_create();
    break;

  default:
    break;
  }
  return NULL;
}

// RcInput vtbl funcs
bool rc_input_get_key_down(RcInput *w, RcKey key) { return w->vtbl->down(w, key); }
bool rc_input_get_key_pressed(RcInput *w, RcKey key) { return w->vtbl->pressed(w, key); }
void rc_input_update(RcInput *w) { w->vtbl->update(w); }
void rc_input_destroy(RcInput *w)
{
  if (w && w->vtbl && w->impl) {
    w->vtbl->destroy(w);
  }
}
