#include "internal/i_alpha.h"
#include "internal/platform/sdl/sdl_input_int.h"

AResult a_input_create(AInputBackendType backend, AInput **out)
{
  switch (backend) {
  case A_INPUT_BACKEND_SDL:
    *out = a_sdl_input_create();
    break;

  default:
    break;
  }
  return A_RES_OK;
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
