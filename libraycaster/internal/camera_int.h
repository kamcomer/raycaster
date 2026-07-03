#ifndef CAMERA_INT_H
#define CAMERA_INT_H

#include "raycaster/types.h"

typedef struct RcCamera AeCamera;

struct RcCamera {
  AeVector pos;
  AeVector dir;
  AeVector plane;
  float fov;
  uint32_t width;
  uint32_t height;
};

AeCamera *rc_camera_create(uint32_t width, uint32_t height);
void ae_camera_destroy(AeCamera *cam);
void ae_camera_set_position(AeCamera *cam, double pos_x, double pos_y);
void ae_camera_set_direction(AeCamera *cam, double dir_x, double dir_y);
void ae_camera_set_fov(AeCamera *cam, float fov_degrees);
void ae_camera_rotate(AeCamera *cam, float angle);
void rc_camera_get_view_dimensions(AeCamera *cam, uint32_t *width, uint32_t *height);

#endif // CAMERA_INT_H
