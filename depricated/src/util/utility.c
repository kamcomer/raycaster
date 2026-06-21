// #include "utility.h"

// // Function to generate points along the circumference of a circle using Bézier
// // curves Vector *
// Vector *generate_circle_points(Vector center, double radius, int numPoints)
// {

//   Vector *points = malloc(numPoints * sizeof(Vector));
//   double step = 2 * PI / numPoints;
//   for (int i = 0; i < numPoints; i++)
//   {
//     double t = i * step;
//     points[i].x = center.x + radius * cos(t);
//     points[i].y = center.y + radius * sin(t);
//   }

//   return points;
// }

// void generate_filled_circle(Renderer *renderer, Vector center,
//                             double radius, int numPoints)
// {
//   Vector *points;
//   for (double r = radius; r > 0; r = r - 1)
//   {
//     SDL_SetRenderDrawColor(renderer, 0, 255 - r * 10, 0, 255);
//     points = generate_circle_points(center, r, numPoints);
//     for (int x = 0; x < numPoints; x++)
//     {
//       SDL_RenderDrawPoint(renderer, points[x].x, points[x].y);
//     }
//     free(points);
//   }
// }

// Texture *draw_filled_circle(Renderer *renderer, Vector center_vect,
//                                 int radius)
// {

//   int diameter = 2 * radius;

//   Surface *surface =
//       SDL_CreateRGBSurface(0, diameter, diameter, 32, 0, 0, 0, 0);

//   SDL_LockSurface(surface);

//   uint32_t *pixels = (uint32_t *)surface->pixels;
//   for (int x = center_vect.x - radius; x <= center_vect.x + radius; x++)
//   {
//     for (int y = center_vect.y - radius; y <= center_vect.y + radius; y++)
//     {
//       int dx = x - center_vect.x;
//       int dy = y - center_vect.y;
//       if (dx * dx + dy * dy <= radius * radius)
//       {
//         int index = (dy + radius) * diameter + (dx + radius);
//         pixels[index] = SDL_MapRGB(surface->format, 255, 0, 0);
//       }
//     }
//   }

//   SDL_UnlockSurface(surface);

//   SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
//   SDL_FreeSurface(surface);

//   return texture;
// }
