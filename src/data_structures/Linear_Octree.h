#ifndef SWARMING_PROJECT_LINEAR_OCTREE_H
#define SWARMING_PROJECT_LINEAR_OCTREE_H

#include "definitions/types.h"
#include "definitions/constants.h"
#include "data_structures/Boid.h"
#include "data_structures/Octree.h"
#include <vector>
#include <iterator>

/**
* Class that represents an octree.
* @tparam m_depth      The depth of our octree
* @tparam Dimension    The dimension of the space (each tree has a maximum of 2**Dimension children).
*/
template <std::size_t Dimension>
class Linear_Octree {

public:
    std::vector<Octree<Dimension>> m_octants;

    /**
    * Constructor for the Linear Octree class.
    * @param l list of octrees to initialize
    */
    Linear_Octree(std::vector<Octree<Dimension>> octants)
        : m_octants(octants)
    {
    }

    /**
    * Removes overlaps from the vector (algo 7 in the paper)
    * The vector needs to be sorted
    */
    void remove_overlaps() {
        for (int i = 1; i < m_octants.size(); i++) {
            if (m_octants.at(i-1).is_ancestor(m_octants.at(i))) {
                m_octants.erase(std::next(m_octants.begin()+i-2));
                i--;
            }
        }
    }
};

#endif //SWARMING_PROJECT_LINEAR_OCTREE_H
