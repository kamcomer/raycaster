#ifndef VECTOR_H
#define VECTOR_H

#define PI 3.14159265358979323846
#define PI_2 (PI / 2.0)
#define PI_3 (PI / 3.0)
#define PI_4 (PI / 4.0)
#define PI_6 (PI / 6.0)
#define PI_8 (PI / 8.0)
#define DEG_TO_RAD (PI / 180.0)

// Structure representing a 2D vector or point with x and y components,
// magnitude (mag), and angle (angle) in radians.
typedef struct
{
  double x;     // X-coordinate
  double y;     // Y-coordinate
  double mag;   // Magnitude of the vector
  double angle; // Angle of the vector in radians
} Vector;

/**
 * @brief Initializes a vector with the given x and y coordinates, calculating
 *        its magnitude and angle.
 *
 * @param x X-coordinate of the vector
 * @param y Y-coordinate of the vector
 * @return Vector Initialized vector with x, y, magnitude, and angle.
 */
Vector set_vector(double x, double y);

/**
 * @brief Initializes a point with the given x and y coordinates. Since this is
 *        a point, magnitude and angle are set to NAN.
 *
 * @param x X-coordinate of the point
 * @param y Y-coordinate of the point
 * @return Vector Initialized point with x, y coordinates, and NAN magnitude and
 * angle.
 */
Vector set_point(double x, double y);

/**
 * @brief Sets the magnitude of the vector and updates its x and y components
 * accordingly. If magnitude is zero, x and y are set to zero and angle is set
 * to NAN.
 *
 * @param vector Pointer to the vector whose magnitude is being set.
 * @param mag The new magnitude to set.
 */
void set_vector_magnitude(Vector *vect, double mag);

/**
 * @brief Calculates the magnitude of the given vector based on its x and y
 * components.
 *
 * @param vector Pointer to the vector whose magnitude is being calculated.
 */
void calculate_vector_mag(Vector *vect);

/**
 * @brief Calculates the angle of the vector based on its x and y components.
 *        The angle is normalized to the range [0, 2π).
 *
 * @param vector Pointer to the vector whose angle is being calculated.
 */
void calculate_vector_angle(Vector *vect);

/**
 * @brief Prints the vector's x, y components, magnitude, and angle.
 *
 * @param vector The vector to be printed.
 */
void print_vector(Vector vect);

/**
 * @brief Rotates the given vector by the specified angle (in radians).
 *        The x and y components are updated, and the angle is normalized to [0,
 * 2π).
 *
 * @param vector Pointer to the vector to be rotated.
 * @param angle The angle (in radians) by which to rotate the vector.
 */
void rotate_vector(Vector *vect, double angle);

/**
 * @brief Translates a vector that is relative to the origin to be relative to a
 * given origin. The translation is performed by adding the origin's coordinates
 * to the vector's coordinates.
 *
 * @param vect The vector to be translated.
 * @param origin The vector or point representing the new origin.
 * @return Vector The translated vector.
 */
void translate_vector(Vector *vect, Vector origin);

/**
 * @brief Normalizes the given vector to a unit vector (magnitude = 1) if its
 * magnitude is not zero. If the magnitude is zero, x, y are set to zero and
 * angle to NAN.
 *
 * @param vector The vector to normalize.
 * @return Vector The normalized vector.
 */
void normalize_vector(Vector *vect);

#endif // VECTOR_H
