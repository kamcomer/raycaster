#ifndef RENDER_2D_H
#define RENDER_2D_H

#include "scene.h"

void render_2d_scene(Scene *scene);
void render_2d_map(Scene scene);
void render_2d_player(Scene *scene);
void render_player_plane(Scene *scene);
void render_actor_body(Scene *scene);
void render_actor_view_dir(Scene *scene);
void render_actor_vel_dir(Scene *scene);
void render_actor_view_rays(Scene *scene);
void render_player_view_rays(Scene *scene);

#endif
