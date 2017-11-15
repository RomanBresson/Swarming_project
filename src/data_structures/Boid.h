#ifndef SWARMING_PROJECT_BOIDS_H
#define SWARMING_PROJECT_BOIDS_H

#include "definitions/types.h"
#include <string>

using types::Vector3Pos;
using types::Vector3Vel;
using types::Vector3Force;


//using namespace types;
struct Boid {

  Vector3Pos    m_position;
  Vector3Vel    m_velocity;
  Vector3Force  m_force;

  Boid(const Vector3Pos & min_corner, const Vector3Pos & max_corner);

  std::string toString() const;

  int get_type() const;

  Vector3Pos compute_center_of_mass(const std::vector<Boid> &);

  void alignment_update(const std::vector<Boid> &);
  void cohesion_update(const std::vector<Boid> &);
  void separation_update(const std::vector<Boid> &);
  void border_force_update(const std::vector<Boid> &);

};

#endif //SWARMING_PROJECT_BOIDS_H
