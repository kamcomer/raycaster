#ifndef RAYCASTER_CAMERA_H
#define RAYCASTER_CAMERA_H

#include "types.h"

typedef struct RcCamera RcCamera;

struct RcCamera
{
    RcVector pos;
    RcVector dir;
    RcVector plane;
    float fov;
    int width;
    int height;
};

RcCamera *rc_camera_create(int width, int height);
void rc_camera_destroy(RcCamera *cam);
void rc_camera_set_position(RcCamera *cam, double x, double y);
void rc_camera_set_direction(RcCamera *cam, double dir_x, double dir_y);
void rc_camera_set_fov(RcCamera *cam, float fov_degrees);
void rc_camera_rotate(RcCamera *cam, float angle);
void rc_camera_get_view_dimensions(RcCamera *cam, int *width, int *height);

#endif // RAYCASTER_CAMERA_H
