#ifndef VECTOR_H
#define VECTOR_H

#include "raycaster/types.h"

typedef RcVector Vector;

static void rc_vector_calc_mag(RcVector *vect);
static void rc_vector_calc_angle(RcVector *vect);
RcVector rc_vector_create(double x, double y);
RcVector rc_vector_from_point(double x, double y);
void rc_vector_set_magnitude(RcVector *vect, double mag);
void rc_vector_translate(RcVector *vect, RcVector origin);
void rc_vector_calc_mag(RcVector *vect);
void rc_vector_calc_angle(RcVector *vect);
void rc_vector_print(RcVector vect);
void rc_vector_normalize(RcVector *vect);
void rc_vector_rotate(RcVector *vect, double angle);
double rc_vector_dot(RcVector a, RcVector b);

#endif
