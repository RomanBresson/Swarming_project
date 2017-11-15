#ifndef SWARMING_PROJECT_BOIDS_H
#define SWARMING_PROJECT_BOIDS_H

#include "definitions/types.h"
#include <random>
#include <string>
#include <ostream>
#include <math.h>

using types::PositionType;
using types::Position;
using types::Velocity;
using types::Force;

const float cohesion_normaliser = 0.01;
const float alignment_normaliser = 0.01;
const float separation_normaliser = 0.01;
const float separation_min_distance = 1.0;


/**
 * Struct that represents a boid agent.
 * @tparam Position     The spatial coordinates of the agent.
 * @tparam Velocity     The coordinates of the agent's velocity.
 * @tparam Force        The sum of all forces applied on the agent.
 */
template <typename Distribution, std::size_t Dimension>
struct Boid {

    Position<Dimension> m_position;
    Velocity<Dimension> m_velocity;
    Force   <Dimension> m_force;

    /**
     * Constructor for the Boid class.
     * @param bottom_left     The bottom-left corner of the space we want to simulate.
     * @param top_right       The top-right corner of the space we want to simulate.
     */
    Boid(Position<Dimension> const & bottom_left,
         Position<Dimension> const & top_right)
    {
        std::random_device d;
        std::default_random_engine generator(d());
        for(std::size_t i{0}; i < Dimension; ++i) {
            Distribution distribution(bottom_left[i], top_right[i]);
            m_position[i] = distribution(generator);
            m_velocity[i] = 0.0;
            m_force[i]    = 0.0;
        }
    }

    /**
      *Computes the force of alignment applied on the boid, then updates the boid's force parameter
      *@tparam neighbours : list of the boids that are close enough to the agent to apply the force
      */
    void alignement_update  (const std::vector<Boid> & neighbours);
    /**
      *Computes the force of cohesion applied on the boid, then updates the boid's force parameter
      *@tparam neighbours : list of the boids that are close enough to the agent to apply the force
      */
    void cohesion_update(const std::vector<Boid> & neighbours) {
        if(neighbours.size() != 0){
            Position<Dimension> center = compute_center_of_mass(neighbours);
            for(int j=0; j<Dimension; j++) {
                m_force[j] += cohesion_normaliser*(center[j] - m_position[j]);
            }
        }
    }
    /**
      *Computes the force of separation applied on the boid, then updates the boid's force parameter
      *@tparam neighbours : list of the boids that are close enough to the agent to apply the force
      */
    void separation_update(const std::vector<Boid> & neighbours) {
        for(auto const & neighbour : neighbours) {
            //TODO : JUST A NAIVE DISTANCE COMPUTING
            float dist = 0.0;
            for(int j=0; j<Dimension; j++) {
                dist += pow(m_position[j]-neighbour.m_position[j], 2);
            }
            if (dist <= pow(separation_min_distance, 2)){
                for(int j=0; j<Dimension; j++) {
                    m_force[j] += separation_normaliser*(m_position[j]-neighbour.m_position[j]);
                }
            }
        }
    }
    /**
      *Computes the force of repulsion applied on the boid by the borders of the grid, then updates the boid's force parameter
      *@tparam neighbours : list of the boids that are close enough to the agent to apply the force
      */
    void border_force_update();

    /**
      *Computes the center of mass of a number of boids
      *@tparam neighbours : list of the boids whose center is to be computed
      */
    Position<Dimension> compute_center_of_mass(const std::vector<Boid> & neighbours) {
        Position<Dimension> center;
        for (int j=0; j<Dimension; j++) {
                center[j] =0.0;
        }
        if (neighbours.size() ==0){ //no neighbours are perceived : no influence must be had on our current boid
            return(Position<Dimension>{-1});
        }
        for(auto const & neighbour : neighbours) {
            for (int j=0; j<Dimension; j++) {
                center[j] += neighbour.m_position[j]/neighbours.size();
                //std::cout << center[j] << "////" << neighbour.m_position[j] << "/////" << neighbours.size() << std::endl;
            }
        }
        return center;
    }
};

template <typename Distribution, std::size_t Dimension>
std::ostream & operator<<(std::ostream & os, const Boid<Distribution, Dimension> & boid) {
    return os << "Position: " << boid.m_position << std::endl
              << "\tVelocity: " << boid.m_velocity << std::endl
              << "\tForce: " << boid.m_force << std::endl;
}

#endif //SWARMING_PROJECT_BOIDS_H
