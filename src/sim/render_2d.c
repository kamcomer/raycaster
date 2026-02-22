#include "render_2d.h"
#include "scene.h"
#include "renderer.h"
#include "window_ctx.h"

void render_2d_scene(Scene *scene)
{
  render_2d_map(*scene);
  render_2d_player(scene);
}

void render_2d_map(Scene scene)
{
  int x_offset;
  int y_offset;

  set_render_draw_color(scene.window_ctx->renderer, 255, 255, 255, 255);
  clear_renderer(scene.window_ctx->renderer);

  for (int y = 0; y < scene.map.height; y++)
  {
    y_offset = (y == scene.map.height - 1) ? 0 : 1;

    for (int x = 0; x < scene.map.width; x++)
    {
      x_offset = (x == scene.map.width - 1) ? 0 : 1;

      if (scene.map.walls[y][x])
      {
        set_render_draw_color(scene.window_ctx->renderer, 255, 0, 0, 255);
      }
      else
      {
        set_render_draw_color(scene.window_ctx->renderer, 0, 0, 0, 255);
      }

      Rect rectangle = {x * scene.map.unit_size, y * scene.map.unit_size,
                        scene.map.unit_size - x_offset,
                        scene.map.unit_size - y_offset};

      render_fill_rect(scene.window_ctx->renderer, &rectangle);
    }
  }
}

void render_2d_player(Scene *scene)
{
  render_actor_body(scene);

#ifdef DEBUG
  render_player_view_rays(scene);
  render_actor_view_dir(scene);
  render_actor_vel_dir(scene);
  render_player_plane(scene);
#endif
}

void render_player_plane(Scene *scene)
{
  Player player = scene->player;
  set_render_draw_color(scene->window_ctx->renderer, 0, 0, 0, 255);
  set_vector_magnitude(&player.actor->dir, 10);
  set_vector_magnitude(&player.plane, 5);
  render_draw_line(scene->window_ctx->renderer,
                   player.actor->pos.x + player.actor->dir.x - player.plane.x,
                   player.actor->pos.y + player.actor->dir.y - player.plane.y,
                   player.actor->pos.x + player.actor->dir.x + player.plane.x,
                   player.actor->pos.y + player.actor->dir.y + player.plane.y);
}

void render_actor_body(Scene *scene)
{
  Actor actor = *scene->player.actor;
  set_render_draw_color(scene->window_ctx->renderer, 0, 255, 0, 255);
  Rect rect = {actor.pos.x - (actor.size >> 1),
               actor.pos.y - (actor.size >> 1), actor.size, actor.size};
  render_fill_rect(scene->window_ctx->renderer, &rect);
}

void render_actor_view_dir(Scene *scene)
{
  Actor actor = *scene->player.actor;
  set_render_draw_color(scene->window_ctx->renderer, 0, 0, 0, 255);
  set_vector_magnitude(&actor.dir, 10);
  translate_vector(&actor.dir, actor.pos);
  render_draw_line(scene->window_ctx->renderer,
                   actor.pos.x, actor.pos.y, actor.dir.x, actor.dir.y);
}

void render_actor_vel_dir(Scene *scene)
{
  Actor actor = *scene->player.actor;
  set_render_draw_color(scene->window_ctx->renderer, 255, 255, 255, 255);
  set_vector_magnitude(&actor.velocity, 10);
  Vector vel_endpoint = set_vector(actor.pos.x + actor.velocity.x,
                                   actor.pos.y + actor.velocity.y);
  render_draw_line(scene->window_ctx->renderer,
                   actor.pos.x, actor.pos.y, vel_endpoint.x, vel_endpoint.y);
}

void render_actor_view_rays(Scene *scene)
{
  Actor actor = *scene->player.actor;
  set_render_draw_color(scene->window_ctx->renderer, 255, 0, 255, 75);
  for (int i = 0; i < DEFAULT_NUM_RAYS; i++)
  {
    Vector ray = actor.view_cone[i];
    render_draw_line(scene->window_ctx->renderer,
                     actor.pos.x, actor.pos.y, ray.x, ray.y);
  }
}

void render_player_view_rays(Scene *scene)
{
  Player player = scene->player;
  set_render_draw_color(scene->window_ctx->renderer, 255, 0, 255, 75);
  for (int i = 0; i < scene->window_ctx->window_config->width; i++)
  {
    Vector ray = player.intersects[i].vect;
    render_draw_line(scene->window_ctx->renderer,
                     player.actor->pos.x, player.actor->pos.y,
                     ray.x * DEFAULT_MAP_UNIT_SIZE, ray.y * DEFAULT_MAP_UNIT_SIZE);
  }
}
