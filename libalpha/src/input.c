#include "internal/i_alpha.h"
#include "internal/platform/sdl.h"

AResult a_input_create(AInputBackendType backend, AInput **out)
{
  AResult res;
  switch (backend) {
  case A_INPUT_BACKEND_SDL:
    res = a_sdl_input_create(out);
    break;

  default:
    break;
  }

  return res;
}

bool a_input_get_key_down(AInput *input_manager, AKey key)
{
  return input_manager->vtbl->down(input_manager, key);
}
bool a_input_get_key_pressed(AInput *input_manager, AKey key)
{
  return input_manager->vtbl->pressed(input_manager, key);
}
void a_input_update(AInput *input_manager) { input_manager->vtbl->update(input_manager); }
void a_input_destroy(AInput *input_manager)
{
  if (input_manager && input_manager->vtbl && input_manager->impl) {
    input_manager->vtbl->destroy(input_manager);
  }
}
