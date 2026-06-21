#ifndef VECTOR_H
#define VECTOR_H

#include "raycaster/types.h"

typedef RcVector Vector;

static inline Vector vector_create(double x, double y) {
    return rc_vector_create(x, y);
}

static inline Vector vector_from_point(double x, double y) {
    return rc_vector_from_point(x, y);
}

static inline void vector_set_magnitude(Vector *v, double mag) {
    rc_vector_set_magnitude(v, mag);
}

static inline void vector_translate(Vector *v, Vector origin) {
    rc_vector_translate(v, origin);
}

static inline void vector_print(Vector v) {
    rc_vector_print(v);
}

static inline void vector_normalize(Vector *v) {
    rc_vector_normalize(v);
}

static inline void vector_rotate(Vector *v, double angle) {
    rc_vector_rotate(v, angle);
}

static inline double vector_dot(Vector a, Vector b) {
    return rc_vector_dot(a, b);
}

#endif
