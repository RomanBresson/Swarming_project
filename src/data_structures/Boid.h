#ifndef SWARMING_PROJECT_BOIDS_H
#define SWARMING_PROJECT_BOIDS_H

#include "definitions/types.h"
#include <random>
#include <string>
#include <ostream>

using types::PositionType;
using types::Position;
using types::Velocity;
using types::Force;

template <typename Distribution, std::size_t Dimension>
struct Boid {

    Position<Dimension> m_position;
    Velocity<Dimension> m_velocity;
    Force   <Dimension> m_force;

    Boid(Position<Dimension> const & bottom_left,
         Position<Dimension> const & top_right)
    {
        std::random_device d;
        std::default_random_engine generator(d());
        for(std::size_t i{0}; i < Dimension; ++i) {
            Distribution distribution(bottom_left[i], top_right[i]);
            this->m_position[i] = distribution(generator);
            this->m_velocity[i] = 0.0;
            this->m_force[i]    = 0.0;
        }
    }

    void alignement_update  (const std::vector<Boid> & neighbours);
    void cohesion_update    (const std::vector<Boid> & neighbours);
    void separation_update  (const std::vector<Boid> & neighbours);
    void border_force_update(const std::vector<Boid> & neighbours);

    int get_type() const;

    Position<Dimension> compute_center_of_mass(const std::vector<Boid> & neighbours) {
        Position<Dimension> center;
        int same_type = 0;
        for(auto const & neighbour : neighbours) {
            if (get_type() == neighbour.get_type()){
                same_type++;
                for (int j=0; j<Dimension; j++) {
                    center[j] += neighbour.m_position[j];
                }
            }
        }
        if (same_type > 0){
            for (int j=0; j<3; j++) {
                center[j] /= same_type;
            }
            return center;
        } else {
            center = {-1,-1,-1};
            return center;
        }
    }

};

template <typename Distribution, std::size_t Dimension>
std::ostream & operator<<(std::ostream & os, const Boid<Distribution, Dimension> & boid) {
    return os << "Position: " << boid.m_position << std::endl
              << "\tVelocity: " << boid.m_velocity << std::endl
              << "\tForce: " << boid.m_force << std::endl;
}

#endif //SWARMING_PROJECT_BOIDS_H
