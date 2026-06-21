#ifndef RENDER_FP_H
#define RENDER_FP_H

#include "scene.h"

void render_fp_scene(Scene *scene);
void renderer_sprites(Scene *scene);
void render_floor_and_ceil(Scene *scene);
void render_walls(Scene *scene);

#endif
