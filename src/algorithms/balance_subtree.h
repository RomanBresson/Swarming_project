#ifndef SWARMING_PROJECT_BALANCE_SUBTREE_H
#define SWARMING_PROJECT_BALANCE_SUBTREE_H

#include <vector>
#include <algorithm>

#include "algorithms/remove_duplicates.h"
#include "algorithms/linearise.h"
#include "data_structures/Octree.h"
#include "definitions/constants.h"

#if SWARMING_DO_ALL_CHECKS == 1
#include <cassert>
#endif

/**
 * Implementation of algorithm n°6.
 *
 * This algorithm balance the subtree whose root node is N.
 * @tparam Dimension the dimension of the simulation (2 for 2D, 3 for 3D).
 * @param N root of the subtree to balance.
 * @param L one of the descendant of @a N.
 * @return balanced subtree.
 */
template <std::size_t Dimension>
std::vector<Octree<Dimension>> balance_subtree(Octree<Dimension> const & N,
                                               Octree<Dimension> const & L)
{
    std::vector< Octree<Dimension> > W{L}, R; // Notations of the article

#if SWARMING_DO_ALL_CHECKS == 1
    assert(N.m_depth != 0);
#endif

    for(std::size_t l{L.m_depth}; l > N.m_depth; --l) {
        std::vector<Octree<Dimension>> T;
        for(Octree<Dimension> const & w : W) {
            // Update of R with w and its siblings.
            R.push_back(w);
            auto const siblings = w.get_siblings();
            R.insert(R.end(), siblings.begin(), siblings.end());

            // Update of T
            std::vector< Octree<Dimension> > father_neighbours = w.get_father().get_siblings();
            for(auto && father_neighbour : father_neighbours) {
                if(N.is_ancestor(father_neighbour))
                    // std::move should not be needed here, keeping it for clarity.
                    T.push_back(std::move(father_neighbour));
            }
        }
        W.insert(W.end(), T.begin(), T.end());
    }

    std::sort(R.begin(), R.end());
    R = remove_duplicates(R);
    R = linearise(R);

    return R;
}


template <std::size_t Dimension>
std::vector<Octree<Dimension>> balance_subtree(Octree<Dimension> const & N,
                                               std::list<Octree<Dimension>> const & L)
{
    std::vector< Octree<Dimension> > W(L.begin(), L.end()), P, R; // Notations of the article

#if SWARMING_DO_ALL_CHECKS == 1
    assert(N.m_depth != 0);
#endif

    for(std::size_t l{constants::Dmax}; l > N.m_depth; --l) {
        std::vector< Octree<Dimension> > Q;
        for(Octree<Dimension> const & octant : W) {
            if(octant.m_depth == l)
                Q.push_back(octant);
        }
        std::sort(Q.begin(), Q.end());
    }

}


#endif //SWARMING_PROJECT_BALANCE_SUBTREE_H
