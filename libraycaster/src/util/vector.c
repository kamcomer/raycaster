#include "raycaster/types.h"

#include <math.h>
#include <stdio.h>

#ifndef M_2PI
#define M_2PI (2.0 * M_PI)
#endif

static void rc_vector_calc_mag(RcVector *vect);
static void rc_vector_calc_angle(RcVector *vect);

RcVector rc_vector_create(double x, double y)
{
    RcVector vect;
    vect.x = x;
    vect.y = y;
    rc_vector_calc_angle(&vect);
    rc_vector_calc_mag(&vect);
    return vect;
}

RcVector rc_vector_from_point(double x, double y)
{
    RcVector vect;
    vect.x = x;
    vect.y = y;
    vect.mag = 0;
    vect.angle = 0;
    return vect;
}

void rc_vector_set_magnitude(RcVector *vect, double mag)
{
    vect->mag = mag;
    if (mag != 0)
    {
        vect->x = mag * cos(vect->angle);
        vect->y = mag * sin(vect->angle);
    }
    else
    {
        vect->x = 0;
        vect->y = 0;
        vect->angle = 0;
    }
}

void rc_vector_translate(RcVector *vect, RcVector origin)
{
    *vect = rc_vector_create(vect->x + origin.x, vect->y + origin.y);
}

void rc_vector_calc_mag(RcVector *vect)
{
    vect->mag = sqrt(vect->x * vect->x + vect->y * vect->y);
}

void rc_vector_calc_angle(RcVector *vect)
{
    vect->angle = atan2(vect->y, vect->x);
    vect->angle = fmod(vect->angle + M_2PI, M_2PI);
}

void rc_vector_print(RcVector vect)
{
    printf("X: %f, Y: %f, Mag: %f, Angle: %f\n", vect.x, vect.y, vect.mag, vect.angle);
}

void rc_vector_normalize(RcVector *vect)
{
    if (vect->mag == 0)
    {
        vect->x = 0;
        vect->y = 0;
        vect->mag = 0;
        vect->angle = 0;
    }
    else
    {
        vect->x /= vect->mag;
        vect->y /= vect->mag;
        vect->mag = 1;
        rc_vector_calc_angle(vect);
    }
}

void rc_vector_rotate(RcVector *vect, double angle)
{
    double newX = vect->x * cos(angle) - vect->y * sin(angle);
    double newY = vect->x * sin(angle) + vect->y * cos(angle);
    vect->x = newX;
    vect->y = newY;

    vect->angle += angle;
    vect->angle = fmod(vect->angle + M_2PI, M_2PI);
}

double rc_vector_dot(RcVector a, RcVector b)
{
    return a.x * b.x + a.y * b.y;
}
