#ifndef TEXTURE_INT_H
#define TEXTURE_INT_H

#include "raycaster/types.h"

RcTextureData *rc_create_textures(void);
void rc_free_textures(RcTextureData *textures);

#endif
