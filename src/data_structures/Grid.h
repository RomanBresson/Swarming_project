#ifndef SWARMING_PROJECT_GRID_H
#define SWARMING_PROJECT_GRID_H

#include "definitions/types.h"
#include "data_structures/Boid.h"

#include <vector>
#include <ostream>

using types::Position;

/**
 * Class that represents a physical space.
 * @tparam Distribution The probability distribution used to create the boids inside the space.
 * @tparam Dimension    The dimension of the space.
 */
template <typename Distribution, std::size_t Dimension>
class Grid {

    template <typename Dist, std::size_t Dim>
    friend std::ostream & operator<<(std::ostream & os, Grid<Dist, Dim> const & grid);

public:

    /**
     * Constructor for the Grid class.
     * @param bottom_left     The bottom-left corner of the space we want to simulate.
     * @param top_right       The top-right corner of the space we want to simulate.
     * @param number_of_boids The number of randomly-distributed boids initially in the grid.
     */
    Grid(Position<Dimension> const & bottom_left, Position<Dimension> const & top_right,
         std::size_t number_of_boids = 0)
            : m_bottom_left(bottom_left),
              m_top_right(top_right)
    {
        m_boids.reserve(number_of_boids);
        for(std::size_t i{0}; i < number_of_boids; ++i) {
            m_boids.emplace_back(bottom_left, top_right);
        }
    }

    /**
     * Add a unique randomly-distributed boid to the grid.
     */
    void add_boid() {
        m_boids.emplace_back(m_bottom_left, m_top_right);
    }

    /**
     * Add multiples randmly-distributed boids to the grid.
     * @param number_of_boids_to_add The number of boids to add to the grid.
     */
    void add_boids(std::size_t number_of_boids_to_add) {
        for(std::size_t i{0}; i < number_of_boids_to_add; ++i) {
            this->add_boid();
        }
    }

    /**
     * Randomly re-create all the boids contained in the Grid.
     */
    void shuffle() {
        for(auto & boid : m_boids) {
            boid = Boid<Distribution, Dimension>(m_bottom_left, m_top_right);
        }
    }

    /**
     * Computes forces, velocity and then position for all boids and updates them
     */
    void update_all_boids() {
        for(std::size_t i{0}; i < m_boids.size(); ++i) {
            std::vector<Boid<Distribution, Dimension> > neighbours;
            for(std::size_t j{0}; j < m_boids.size(); ++j){
                if(i != j && m_boids[i].is_visible(m_boids[j])){
                    neighbours.push_back(m_boids[j]);
                }
            }
            m_boids[i].update_forces(neighbours, m_bottom_left, m_top_right);
            m_boids[i].update_velocity(neighbours);
        }
        for(auto & boid : m_boids) {
            boid.update_position(m_bottom_left, m_top_right);
        }
    }

//private:

    /**
     * The bottom-left corner of the space (i.e. the point of the space with the lowest
     * coordinates).
     */
    Position<Dimension> m_bottom_left;

    /**
     * The top-right corner of the space (i.e. the point of the space with the highest
     * coordinates).
     */
    Position<Dimension> m_top_right;

    /**
     * All the boids contained in the space represented by this instance.
     */
    std::vector< Boid<Distribution, Dimension> > m_boids;

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
