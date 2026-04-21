#ifndef VECTOR_H
#define VECTOR_H

#include "raycaster/types.h"

typedef RcVector Vector;

static inline Vector set_vector(double x, double y) {
    return rc_vector_create(x, y);
}

static inline Vector set_point(double x, double y) {
    return rc_vector_from_point(x, y);
}

static inline void set_vector_magnitude(Vector *v, double mag) {
    rc_vector_set_magnitude(v, mag);
}

static inline void rotate_vector(Vector *v, double angle) {
    rc_vector_rotate(v, angle);
}

static inline void normalize_vector(Vector *v) {
    rc_vector_normalize(v);
}

static inline void translate_vector(Vector *v, Vector origin) {
    rc_vector_translate(v, origin);
}

static inline double vector_dot(Vector a, Vector b) {
    return rc_vector_dot(a, b);
}

static inline double vector_angle(Vector a, Vector b) {
    return rc_vector_angle_between(a, b);
}

#endif
