#ifndef INPUT_INT_H
#define INPUT_INT_H

#include "raycaster/input.h"

typedef struct RcInputVtbl RcInputVtbl;

struct RcInput {
  const RcInputVtbl *vtbl;
  void *impl;
};

struct RcInputVtbl {
  bool (*down)(AeInput *input, RcKey key);
  bool (*pressed)(AeInput *input, RcKey key);
  void (*update)(AeInput *input);
  void (*destroy)(AeInput *input);
};

#endif // INPUT_INT_H
