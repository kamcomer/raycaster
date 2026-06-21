#ifndef UTILITY_H
#define UTILITY_H

#include "vector.h"
#include "texture.h"
#include "renderer.h"
#include <stdlib.h>

Vector *generate_circle_points(Vector center, double radius, int numPoints);
void generate_filled_circle(Renderer *renderer, Vector center,
                            double radius, int numPoints);

Texture *draw_filled_circle(Renderer *renderer, Vector center_vect,
                            int radius);
#endif
