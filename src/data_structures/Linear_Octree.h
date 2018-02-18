#ifndef SWARMING_PROJECT_LINEAR_OCTREE_H
#define SWARMING_PROJECT_LINEAR_OCTREE_H

#include <vector>
#include <list>
#include <algorithm>
#include <iterator>
#include "algorithms/sample_sort.h"

#include "definitions/types.h"
#include "definitions/constants.h"
#include "mpi.h"
#include "data_structures/Octree.h"

#include "algorithms/complete_region.h"
#include "algorithms/complete_octree.h"
#include "algorithms/remove_duplicates.h"

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
    explicit Linear_Octree(std::vector<Octree<Dimension>> octants)
        : m_octants(octants)
    { }

    /**
    * Constructor for the Linear Octree class (algorithm 3).
    * @param a : first octant
    * @param b : last octant
    */
    Linear_Octree(Octree<Dimension> a, Octree<Dimension> b)
            : m_octants{complete_region(a, b)}
    { }

    /**
    * Constructor for the Linear Octree class (algorithm 4).
    * @param L : partial TODO sorted list of octants
    */
    explicit Linear_Octree(std::list<Octree<Dimension>> partial_list)
            : m_octants{complete_octree(partial_list)}
    { }

};

#endif //SWARMING_PROJECT_LINEAR_OCTREE_H
