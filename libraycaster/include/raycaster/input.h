#ifndef RAYCASTER_INPUT_H
#define RAYCASTER_INPUT_H

#include "types.h"
#include <stdbool.h>

typedef struct RcInput RcInput;

typedef enum
{
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

RcInput *rc_input_create(void);
void rc_input_destroy(RcInput *in);
bool rc_input_down(RcInput *in, RcKey key);
bool rc_input_pressed(RcInput *in, RcKey key);
void rc_input_update(RcInput *in);

#endif // RAYCASTER_INPUT_H
