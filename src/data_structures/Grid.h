#ifndef SWARMING_PROJECT_GRID_H
#define SWARMING_PROJECT_GRID_H

#include "definitions/types.h"
#include "definitions/constants.h"
#include "data_structures/Boid.h"
#include <random>
#include <vector>
#include <ostream>

using types::Position;
using namespace constants;

/**
 * Class that represents a physical space.
 * @tparam Distribution The probability distribution used to create the boids inside the space.
 * @tparam Dimension    The dimension of the space.
 */
template <typename Distribution, std::size_t Dimension>
class Grid {

public:

    /**
     * Constructor for the Grid class.
     * @param number_of_boids The number of randomly-distributed boids initially in the grid.
     */
    explicit Grid(std::size_t number_of_boids = 0)
    {
        add_boids(number_of_boids);
    }

    /**
     * Add a unique randomly-distributed boid to the grid.
     */
     void add_boid(Position<Dimension> pos, Velocity<Dimension> vel, Force<Dimension> force) {
        m_boids.emplace_back(pos, vel, force);
    }

    /**
     * Add multiples randmly-distributed boids to the grid.
     * @param number_of_boids_to_add The number of boids to add to the grid.
     */
    void add_boids(std::size_t number_of_boids_to_add) {
        m_boids.reserve(number_of_boids_to_add);
        std::random_device d;
        std::default_random_engine generator(d());
        Distribution distribution_pos(BORDER_SEPARATION_MIN_DISTANCE,
                                      GRID_SIZE-BORDER_SEPARATION_MIN_DISTANCE);
        Distribution distribution_vel(-MAX_SPEED,MAX_SPEED);
        for(std::size_t j{0}; j < number_of_boids_to_add; ++j) {
            Position<Dimension> pos;
            Velocity<Dimension> vel;
            Force<Dimension>  force;

            for(std::size_t i{0}; i < Dimension; ++i) {
                pos[i]   = distribution_pos(generator);
                vel[i]   = distribution_vel(generator);
                force[i] = 0.0;
            }
            const double velocity_norm = vel.norm();
            if (velocity_norm > MAX_SPEED) {
                vel *= MAX_SPEED / velocity_norm;
            }
            this->add_boid(pos, vel, force);
        }
    }

    /**
     * Computes forces, velocity and then position for all boids and updates them
     */
    void update_all_boids() {
        for(std::size_t i{0}; i < m_boids.size(); ++i) {
            std::vector<Boid<Dimension> > neighbours;
            for(std::size_t j{0}; j < m_boids.size(); ++j){
                if(i != j && m_boids[i].is_visible(m_boids[j])){
                    neighbours.push_back(m_boids[j]);
                }
            }
            m_boids[i].update_forces(neighbours);
            m_boids[i].update_velocity(neighbours);
        }
        for(auto & boid : m_boids) {
            boid.update_position();
        }
    }

    /**
     * All the boids contained in the space represented by this instance.
     */
    std::vector< Boid<Dimension> > m_boids;

};

template<typename Dist, std::size_t Dim>
std::ostream &operator<<(std::ostream &os, Grid<Dist, Dim> const &grid) {
    os << "Number of boids: " << grid.m_boids.size() << std::endl
       << "Boids:" << std::endl;
    for(auto const & boid : grid.m_boids) {
        os << "\t" << boid << std::endl;
    }
    return os;
}


#endif //SWARMING_PROJECT_GRID_H
