#include "internal/input_manager_int.h"
#include "internal/platform/sdl/sdl_input_int.h"

AeInput *ae_input_create(RcInputBackend backend)
{
  switch (backend) {
  case RC_INPUT_BACKEND_SDL:
    return ae_sdl_input_create();
    break;

  default:
    break;
  }
  return NULL;
}

bool ae_input_get_key_down(AeInput *input_manager, RcKey key)
{
  return input_manager->vtbl->down(input_manager, key);
}
bool ae_input_get_key_pressed(AeInput *input_manager, RcKey key)
{
  return input_manager->vtbl->pressed(input_manager, key);
}
void ae_input_update(AeInput *input_manager) { input_manager->vtbl->update(input_manager); }
void ae_input_destroy(AeInput *input_manager)
{
  if (input_manager && input_manager->vtbl && input_manager->impl) {
    input_manager->vtbl->destroy(input_manager);
  }
}
