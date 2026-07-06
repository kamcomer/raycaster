#ifndef RAYCASTER_INPUT_H
#define RAYCASTER_INPUT_H

#include <stdbool.h>

typedef struct RcInput AeInput;

typedef enum {
  RC_KEY_UNKNOWN = 0,
  RC_KEY_W,
  RC_KEY_S,
  RC_KEY_A,
  RC_KEY_D,
  RC_KEY_UP,
  RC_KEY_DOWN,
  RC_KEY_LEFT,
  RC_KEY_RIGHT,
  RC_KEY_SPACE,
  RC_KEY_ESCAPE,
  RC_KEY_ENTER,
  RC_KEY_SHIFT,
  RC_KEY_CTRL,
  RC_KEY_1,
  RC_KEY_2,
  RC_KEY_3,
  RC_KEY_4,
  RC_KEY_5,
  RC_KEY_6,
  RC_KEY_7,
  RC_KEY_8,
  RC_KEY_9,
  RC_KEY_0,
} RcKey;

typedef enum {
  RC_INPUT_BACKEND_SDL,
} RcInputBackend;

AeInput *ae_input_create(RcInputBackend backend);
bool ae_input_get_key_down(AeInput *input, RcKey key);
bool ae_input_get_key_pressed(AeInput *input, RcKey key);
void ae_input_update(AeInput *input);
void ae_input_destroy(AeInput *input);

#endif // RAYCASTER_INPUT_H
