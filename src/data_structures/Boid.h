#ifndef SWARMING_PROJECT_BOIDS_H
#define SWARMING_PROJECT_BOIDS_H

#include "definitions/types.h"

using types::Vector3Pos;
using types::Vector3Vel;
using types::Vector3Force;

struct Boid {

  Vector3Pos position;
  Vector3Vel Velocity;
  Vector3Force force;

  Boid();

  void alignement_update(const Grid &);
  void cohesion_update(const Grid &);
  void separation_update(const Grid &);
  void border_force_update(const Grid &);

};

#endif //SWARMING_PROJECT_BOIDS_H
