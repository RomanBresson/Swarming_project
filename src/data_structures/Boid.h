#ifndef SWARMING_PROJECT_BOIDS_H
#define SWARMING_PROJECT_BOIDS_H

#include "definitions/types.h"
#include "definitions/constants.h"
#include <string>
#include <ostream>
#include <cmath>
#include <vector>

using types::PositionType;
using types::Position;
using types::Velocity;
using types::Distance;
using types::Force;
using types::DistanceType;

using namespace constants;

/**
 * Struct that represents a boid agent.
 * @tparam Position     spatial coordinates of the agent.
 * @tparam Velocity     coordinates of the agent's velocity.
 * @tparam Force        sum of all forces applied on the agent.
 */
template <std::size_t Dimension>
class Boid {

    using ThisType = Boid<Dimension>;

public:

    Position<Dimension> m_position;
    Velocity<Dimension> m_velocity;
    Force   <Dimension> m_force;

    /**
     * Constructor for the Boid class.
     * @param bottom_left     bottom-left corner of the space we want to simulate.
     * @param top_right       top-right corner of the space we want to simulate.
     */
    Boid(Position<Dimension> const & pos,
         Velocity<Dimension> const & vel,
         Force   <Dimension> const & forc):
         m_position(pos),
         m_velocity(vel),
         m_force(forc)
    {
    }

    /**
     * Computes the force of alignment applied on the boid, then updates the boid's force parameter.
     * @param neighbours list of the boids that are close enough to the agent to apply the force.
     */
    void alignment_update(const std::vector<Boid<Dimension>> & neighbours) {
        Force<Dimension> alignement(0.0);
        const auto multiplier = ALIGNMENT_NORMALISER / neighbours.size();
        for(const Boid<Dimension> & neighbour : neighbours) {
            alignement += multiplier * neighbour.m_velocity;
        }
        m_force += alignement;
    }

    /**
     * Computes the force of cohesion applied on the boid, then updates the boid's force parameter.
     * @param neighbours list of the boids that are close enough to the agent to apply the force.
     */
    void cohesion_update(const std::vector<Boid<Dimension>> & neighbours) {
        if(! neighbours.empty()) {
            Position<Dimension> center = compute_center_of_mass(neighbours);
            Distance<Dimension> direction = center - m_position;

            m_force += COHESION_NORMALISER * direction;
        }
    }

    /**
     * Computes the force of separation applied on the boid, then updates the boid's force parameter.
     * @param neighbours list of the boids that are close enough to the agent to apply the force.
     * @todo For the moment the force is linear. We probably want to change it to inverse of the distance between the
     * two boids.
     */
    void separation_update(const std::vector<Boid<Dimension>> & neighbours) {
        Force<Dimension> separation(0.0);
        for(const Boid<Dimension> & neighbour : neighbours) {
            const Distance<Dimension> to_neighbour = neighbour.m_position - m_position;
            if(to_neighbour.norm() < REPULSION_DISTANCE) {
                separation -= SEPARATION_NORMALISER * to_neighbour;
            }
        }
        m_force += separation;
    }

    /**
     * Computes the force of repulsion applied on the boid by the borders of the grid, then updates the boid's
     * force parameter.
     * @param neighbours list of the boids that are close enough to the agent to apply the force.
     * @todo For the moment the force is linear. We probably want to change it to inverse of the distance between the
     * two boids.
     */
    void border_force_update(){
        const Distance<Dimension> dist1 = m_position;
        Position<Dimension> top_left = m_position;
        for(std::size_t i{0}; i < Dimension; ++i) {
            top_left[i] -= (float)GRID_SIZE;
        }
        const Distance<Dimension> dist2 = top_left;

        Force<Dimension> border_separation(0.0);// = BORDER_SEPARATION_NORMALISER * (1.0/(dist1) + 1.0/(dist2));

        for(std::size_t i{0}; i < Dimension; ++i) {
            if (std::abs(dist1[i]) < BORDER_SEPARATION_MIN_DISTANCE) {
                border_separation[i] += BORDER_SEPARATION_NORMALISER * 1.0/dist1[i];
            }
            if (std::abs(dist2[i]) < BORDER_SEPARATION_MIN_DISTANCE) {
                border_separation[i] += BORDER_SEPARATION_NORMALISER * 1.0/dist2[i];
            }
        }
        m_force += border_separation;
    }

    /**
     * Computes the center of mass of a number of boids
     * @param neighbours list of the boids whose center is to be computed
     */
    Position<Dimension> compute_center_of_mass(const std::vector<Boid<Dimension>> & neighbours) {
        Position<Dimension> center(0.0);
        for(const Boid<Dimension> & neighbour : neighbours) {
            center += neighbour.m_position;
        }
        return center / neighbours.size();
    }

    /**
     * Updates all forces at once
     * @param neighbours  list of the boids who influence the current agent
     * @param bottom_left bottom-left corner of the space we want to simulate.
     * @param top_right   top-right corner of the space we want to simulate.
     */
    void update_forces(const std::vector<Boid> & neighbours) {
        for (int j=0; j<Dimension; j++) {
            m_force[j] = 0.0;
        }
        cohesion_update(neighbours);
        separation_update(neighbours);
        border_force_update();
        alignment_update(neighbours);
    }

    /**
     * updates velocity from forces
     * @param neighbours list of the boids who influence the current agent
     */
    void update_velocity(const std::vector<Boid> & neighbours) {
        m_velocity += TIMESTEP * m_force;
        const auto velocity_norm = m_velocity.norm();

        if (velocity_norm > MAX_SPEED) {
            m_velocity *= MAX_SPEED / velocity_norm;
        }
    }

    /**
     * updates velocity from forces
     * @param neighbours list of the boids who influence the current agent
     */
    void update_position() {
        m_position += TIMESTEP * m_velocity;
//        for (int j=0; j<Dimension; j++) {
//            m_position[j] += m_velocity[j]*TIMESTEP;
//            if (m_position[j] >= top_right[j] - 0.1*BORDER_SEPARATION_MIN_DISTANCE){
//                m_position[j] = top_right[j] - 0.1*BORDER_SEPARATION_MIN_DISTANCE;
//            }
//            if (m_position[j] <= bottom_left[j] + 0.1*BORDER_SEPARATION_MIN_DISTANCE){
//                m_position[j] = bottom_left[j] + 0.1*BORDER_SEPARATION_MIN_DISTANCE;
//            }
//        }
    }

//private:

    /**
     * Tell wether or not a given boid is visible by the current instance.
     * @tparam D   probability distribution of the other boid.
     * @param boid a boid.
     * @return     true if the given boid is visible by the current instance, false otherwise.
     */
    bool is_visible(const Boid<Dimension> & boid) {
        DistanceType const squared_distance{ this->squared_euclidian_distance(boid) };
        if (squared_distance <= VISION_DISTANCE*VISION_DISTANCE) {
            return compute_angle(boid) < VISION_ANGLE;
        }
        return false;
    }

    /**
     * Computes the scalar product between the orientation of the current boid and its relative position to the target boid
     * @tparam D   probability distribution of the other boid.
     * @param boid the other boid.
     * @return     normalized scalar product .
     */
    float scalar_product(const Boid<Dimension> & boid) {
        const Distance<Dimension> current_to_other = boid.m_position - m_position;
        const Distance<Dimension> piecewise_product = m_velocity * current_to_other;
        const auto norm_self = m_velocity.norm();
        const auto norm_other = current_to_other.norm();
        float scalar_product{0.0};
        for (std::size_t j{0}; j < Dimension; ++j) {
            scalar_product += piecewise_product[j];
        }

        if (norm_self != 0.0) {
            scalar_product /= norm_self;
        }
        if (norm_other != 0.0) {
            scalar_product /= norm_other;
        }

        return scalar_product;
    }

    /**
     * Computes the angle between the orientation of the current boid and its relative position to the target boid
     * @tparam D   probability distribution of the other boid.
     * @param boid the other boid.
     * @return     angle between a boid's orientation and another boid.
     */
    float compute_angle(const Boid<Dimension> & boid) {
        return static_cast<float>(std::acos(scalar_product(boid)) * 180.0 / (PI));
    }

    /**
     * Compute the (squared) euclidian distance between the current instance and the given boid.
     * @tparam D   probability distribution of the other boid.
     * @param boid the other boid.
     * @return     distance between the two boids.
     */
    DistanceType squared_euclidian_distance(const Boid<Dimension> & boid) {
        DistanceType distance{DistanceType()};
        for(std::size_t i{0}; i < Dimension; ++i) {
            const PositionType diff{m_position[i] - boid.m_position[i]};
            distance += diff*diff;
        }
        return distance;
    }

};

template <std::size_t Dimension>
std::ostream & operator<<(std::ostream & os, const Boid<Dimension> & boid) {
    return os << "Position: " << boid.m_position << std::endl
              << "\tVelocity: " << boid.m_velocity << std::endl
              << "\tForce: " << boid.m_force << std::endl;
}

template <std::size_t Dimension>
bool operator!=(const Boid<Dimension> & lhs, const Boid<Dimension> & rhs) {
    for(std::size_t i{0}; i < Dimension; ++i) {
        if( std::abs(lhs.m_position[i] - rhs.m_position[i]) > constants::DOUBLE_EPSILON ) {
            return false;
        }
    }
    return true;
};

#endif //SWARMING_PROJECT_BOIDS_H
