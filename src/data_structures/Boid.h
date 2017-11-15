#ifndef SWARMING_PROJECT_BOIDS_H
#define SWARMING_PROJECT_BOIDS_H

#include "definitions/types.h"
#include <random>
#include <string>
#include <ostream>
#include <cmath>

using types::PositionType;
using types::Position;
using types::Velocity;
using types::Force;
using types::DistanceType;

constexpr float COHESION_NORMALISER = 0.01;
constexpr float ALIGNEMENT_NORMALISER = 0.01;
constexpr float SEPARATION_NORMALISER = 0.01;
constexpr float SEPARATION_MIN_DISTANCE = 1.0;


/**
 * Struct that represents a boid agent.
 * @tparam Position     spatial coordinates of the agent.
 * @tparam Velocity     coordinates of the agent's velocity.
 * @tparam Force        sum of all forces applied on the agent.
 */
template <typename Distribution, std::size_t Dimension>
class Boid {

public:

    /**
     * Constructor for the Boid class.
     * @param bottom_left     bottom-left corner of the space we want to simulate.
     * @param top_right       top-right corner of the space we want to simulate.
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
     * Computes the force of alignment applied on the boid, then updates the boid's force parameter.
     * @param neighbours list of the boids that are close enough to the agent to apply the force.
     */
    void alignement_update  (const std::vector<Boid> & neighbours);

    /**
     * Computes the force of cohesion applied on the boid, then updates the boid's force parameter.
     * @param neighbours list of the boids that are close enough to the agent to apply the force.
     */
    void cohesion_update(const std::vector<Boid> & neighbours) {
        if(neighbours.size() != 0){
            Position<Dimension> center = compute_center_of_mass(neighbours);
            for(std::size_t j{0}; j < Dimension; ++j) {
                m_force[j] += COHESION_NORMALISER*(center[j] - m_position[j]);
            }
        }
    }
    /**
     * Computes the force of separation applied on the boid, then updates the boid's force parameter.
     * @param neighbours list of the boids that are close enough to the agent to apply the force.
     * @todo For the moment the force is linear. We probably want to change it to inverse of the distance between the
     * two boids.
     */
    void separation_update(const std::vector<Boid> & neighbours) {
        for(auto const & neighbour : neighbours) {
            if(is_visible(neighbour)) { //Shouldn't we only pass those as arguments in neighbours ?
                for(std::size_t j{0}; j < Dimension; ++j) {
                    m_force[j] += SEPARATION_NORMALISER*(m_position[j]-neighbour.m_position[j]);
                }
            }
        }
    }
    /**
     * Computes the force of repulsion applied on the boid by the borders of the grid, then updates the boid's
     * force parameter.
     * @param neighbours list of the boids that are close enough to the agent to apply the force.
     * @todo For the moment the force is linear. We probably want to change it to inverse of the distance between the
     * two boids.
     */
    void border_force_update(Position<Dimension> const & bottom_left, Position<Dimension> const & top_right);

    /**
     * Computes the center of mass of a number of boids
     * @param neighbours list of the boids whose center is to be computed
     */
    Position<Dimension> compute_center_of_mass(const std::vector<Boid> & neighbours) {
        if (neighbours.size() == 0){ // no neighbours are perceived : no influence must be had on our current boid
            return(Position<Dimension>{-1});
        }
        Position<Dimension> center{};
        for(auto const & neighbour : neighbours) {
            for (int j=0; j<Dimension; j++) {
                center[j] += neighbour.m_position[j]/neighbours.size();
                //std::cout << center[j] << "////" << neighbour.m_position[j] << "/////" << neighbours.size() << std::endl;
            }
        }
        return center;
    }

private:

    /**
     * Tell wether or not a given boid is visible by the current instance.
     * @tparam D   probability distribution of the other boid.
     * @param boid a boid.
     * @return     true if the given boid is visible by the current instance, false otherwise.
     */
    template <typename D>
    bool is_visible(const Boid<D, Dimension> & boid) {
        DistanceType const squared_distance{ this->squared_euclidian_distance(boid) };
        return squared_distance <= SEPARATION_MIN_DISTANCE * SEPARATION_MIN_DISTANCE;
    }

    /**
     * Compute the (squared) euclidian distance between the current instance and the given boid.
     * @tparam D   probability distribution of the other boid.
     * @param boid the other boid.
     * @return     distance between the two boids.
     */
    template <typename D>
    DistanceType squared_euclidian_distance(const Boid<D, Dimension> & boid) {
        DistanceType distance{DistanceType()};
        for(std::size_t i{0}; i < Dimension; ++i) {
            const PositionType diff{m_position[i] - boid.m_position[i]};
            distance += diff*diff;
        }
        return distance;
    }

    Position<Dimension> m_position;
    Velocity<Dimension> m_velocity;
    Force   <Dimension> m_force;

};

template <typename Distribution, std::size_t Dimension>
std::ostream & operator<<(std::ostream & os, const Boid<Distribution, Dimension> & boid) {
    return os << "Position: " << boid.m_position << std::endl
              << "\tVelocity: " << boid.m_velocity << std::endl
              << "\tForce: " << boid.m_force << std::endl;
}

#endif //SWARMING_PROJECT_BOIDS_H
