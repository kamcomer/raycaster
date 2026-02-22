#ifndef PLAYER_H
#define PLAYER_H

#include "window_ctx.h"
#include "actor.h"
#include "vector.h"
#include "input.h"

// Player defines
#define PLAYER_SIZE 7
#define PLAYER_ACCEL 0.01
#define PLAYER_TURN_SPEED 0.05
#define PLAYER_MAX_SPEED 5
#define PLAYER_FOV 60

typedef enum
{
  UP = SCANCODE_W,
  DOWN = SCANCODE_S,
  LEFT = SCANCODE_A,
  RIGHT = SCANCODE_D,
  NONE = SCANCODE_UNKNOWN
} Direction;

typedef struct
{
  int size;
  float accel;
  float turn_speed;
  int max_speed;
  int fov;
} PlayerConfig;

typedef struct
{
  double perp_wall_distance;
  int side;
  int map_x;
  int map_y;
  Vector vect;
  Vector ray_dir;
} WallIntersect;

typedef struct
{
  Actor *actor;
  Vector plane;
  WallIntersect *intersects;
  WindowCtx *window_ctx;
} Player;

Player create_player(WindowCtx *window_ctx);
void update_player(Player *player, float delta_time);
void free_player(Player *player);
void cast_player_rays(Player *player, Map map);
void process_player_motion(Player *player, float delta_time, Map map);
WallIntersect get_wall_intersect(Vector origin, Vector ray, Map map);

#endif
