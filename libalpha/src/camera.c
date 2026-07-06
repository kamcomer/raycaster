#include "alpha.h"
#include "internal/i_alpha.h"
#include <math.h>
#include <stdlib.h>

#define RC_DEFAULT_FOV 60.0f

AResult a_camera_create(uint width, uint height, ACamera **out)
{
  ACamera *cam = calloc(1, sizeof(ACamera));
  if (!cam) {
    return A_RES_INVLD_ARG;
  }

  cam->width = width;
  cam->height = height;
  cam->fov = RC_DEFAULT_FOV;

  cam->pos.x = 0;
  cam->pos.y = 0;
  cam->dir.x = -1;
  cam->dir.y = 0;
  cam->plane.x = 0;
  cam->plane.y = cam->dir.x * tan(cam->fov * A_DEG_TO_RAD / 2);

  *out = cam;
  return A_RES_OK;
}

void a_camera_destroy(ACamera *cam) { free(cam); }

void a_camera_set_position(ACamera *cam, double x, double y)
{
  cam->pos.x = x;
  cam->pos.y = y;
}

void a_camera_set_direction(ACamera *cam, double dir_x, double dir_y)
{
  cam->dir.x = dir_x;
  cam->dir.y = dir_y;

  double fov_rad = cam->fov * A_DEG_TO_RAD / 2;
  cam->plane.x = -dir_y * tan(fov_rad);
  cam->plane.y = dir_x * tan(fov_rad);
}

void a_camera_set_fov(ACamera *cam, double fov_degrees)
{
  cam->fov = fov_degrees;
  double fov_rad = fov_degrees * A_DEG_TO_RAD / 2;
  double len = sqrt((cam->plane.x * cam->plane.x) + (cam->plane.y * cam->plane.y));
  if (len > 0) {
    cam->plane.x = -cam->dir.y * tan(fov_rad);
    cam->plane.y = cam->dir.x * tan(fov_rad);
  }
}

void a_camera_rotate(ACamera *cam, double angle)
{
  double cos_a = cos(angle);
  double sin_a = sin(angle);

  double new_dir_x = (cam->dir.x * cos_a) - (cam->dir.y * sin_a);
  double new_dir_y = (cam->dir.x * sin_a) + (cam->dir.y * cos_a);
  cam->dir.x = new_dir_x;
  cam->dir.y = new_dir_y;

  double new_plane_x = (cam->plane.x * cos_a) - (cam->plane.y * sin_a);
  double new_plane_y = (cam->plane.x * sin_a) + (cam->plane.y * cos_a);
  cam->plane.x = new_plane_x;
  cam->plane.y = new_plane_y;
}

void a_camera_get_view_dimension(ACamera *cam, ADimensions *dim)
{
  if (dim->width) {
    dim->width = cam->width;
  }
  if (dim->height) {
    dim->height = cam->height;
  }
}
