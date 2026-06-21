#include "vector.h"
#include <stdio.h>

#define _USE_MATH_DEFINES
#include <math.h>

// Define a constant for 2*PI
#ifndef M_2PI
#define M_2PI (2.0 * M_PI)
#endif

// Creates and returns a vector with the given x and y coordinates.
// It calculates the vector's angle and magnitude.
Vector set_vector(double x, double y)
{
  Vector vect;
  vect.x = x;
  vect.y = y;
  calculate_vector_angle(&vect); // Calculate angle based on x and y
  calculate_vector_mag(&vect);   // Calculate magnitude based on x and y
  return vect;
}

// Creates and returns a point with the given x and y coordinates.
// Magnitude and angle are set to NAN since this is just a point, not a full
// vector.
Vector set_point(double x, double y)
{
  Vector vect;
  vect.x = x;
  vect.y = y;
  vect.mag = NAN;   // Points do not have a defined magnitude
  vect.angle = NAN; // Points do not have a defined angle
  return vect;
}

// Sets the magnitude of the given vector and updates its x and y components
// accordingly. If magnitude is zero, x and y are set to zero, and the angle is
// set to NAN.
void set_vector_magnitude(Vector *vector, double mag)
{
  vector->mag = mag;
  if (mag != 0)
  {
    vector->x =
        mag *
        cos(vector->angle); // Update x based on the new magnitude and angle
    vector->y =
        mag *
        sin(vector->angle); // Update y based on the new magnitude and angle
  }
  else
  {
    vector->x = 0; // If magnitude is zero, x and y must be zero
    vector->y = 0;
    vector->angle = NAN; // Angle is undefined for a zero-magnitude vector
  }
}

void translate_vector(Vector *vect, Vector origin)
{
  *vect = set_vector(vect->x + origin.x, vect->y + origin.y);
}

// Calculates and sets the magnitude of the given vector based on its x and y
// components.
void calculate_vector_mag(Vector *vect)
{
  vect->mag =
      sqrt(vect->x * vect->x + vect->y * vect->y); // Pythagorean theorem
}

// Calculates and sets the angle of the given vector based on its x and y
// components. Ensures the angle is normalized to the range [0, 2π).
void calculate_vector_angle(Vector *vect)
{
  vect->angle = atan2(
      vect->y, vect->x);                          // Calculate angle using atan2 for full 360° range
  vect->angle = fmod(vect->angle + M_2PI, M_2PI); // Normalize to [0, 2π)
}

// Prints the vector's components, magnitude, and angle to the console.
void print_vector(Vector vect)
{
  printf("X: %f, Y: %f, Mag: %f, Angle: %f\n", vect.x, vect.y, vect.mag,
         vect.angle);
}

// Normalizes the vector to a unit vector (magnitude = 1) if its magnitude is
// not zero. If the vector has zero magnitude, its components are set to zero
// and the angle to NAN.
void normalize_vector(Vector *vect)
{
  Vector norm_vect = *vect;
  if (vect->mag == 0)
  {
    vect->x = 0;
    vect->y = 0;
    vect->mag = 0;
    vect->angle = 0;
  }
  else
  {
    vect->x /= vect->mag;         // Normalize x component
    vect->y /= vect->mag;         // Normalize y component
    vect->mag = 1;                // Set magnitude to 1 for a unit vector
    calculate_vector_angle(vect); // Recalculate the angle
  }
}

// Rotates the vector by the given angle (in radians).
// Updates the vector's x and y components and ensures the angle is normalized
// to [0, 2π).
void rotate_vector(Vector *vect, double angle)
{
  double newX =
      vect->x * cos(angle) - vect->y * sin(angle); // Rotation matrix for x
  double newY =
      vect->x * sin(angle) + vect->y * cos(angle); // Rotation matrix for y
  vect->x = newX;
  vect->y = newY;

  vect->angle += angle; // Update the vector's angle by adding the rotation
  vect->angle =
      fmod(vect->angle + M_2PI, M_2PI); // Normalize the angle to [0, 2π)
}
