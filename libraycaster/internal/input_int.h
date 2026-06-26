#ifndef INPUT_INT_H
#define INPUT_INT_H

#include "raycaster/input.h"

typedef struct RcInputVtbl RcInputVtbl;

struct RcInput {
  const RcInputVtbl *vtbl;
  void *impl;
};

struct RcInputVtbl {
  bool (*down)(RcInput *in, RcKey key);
  bool (*pressed)(RcInput *in, RcKey key);
  void (*update)(RcInput *in);
  void (*destroy)(RcInput *in);
};

#endif // INPUT_INT_H
