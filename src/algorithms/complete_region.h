#ifndef SWARMING_PROJECT_COMPLETE_REGION_H
#define SWARMING_PROJECT_COMPLETE_REGION_H

#include "data_structures/Octree.h"
#include "definitions/constants.h"
#include <vector>
#include <queue>

#if SWARMING_DO_ALL_CHECKS == 1
#include <cassert>
#endif

/**
* Constructor for the Linear Octree class (algorithm 3).
* @param a : first octant
* @param b : last octant
*/
template <std::size_t Dimension>
std::vector< Octree<Dimension> > complete_region(Octree<Dimension> a, Octree<Dimension> b) {

#if SWARMING_DO_ALL_CHECKS == 1
    assert(a < b);
#endif

    std::vector< Octree<Dimension> > completed_region;

    auto morton_a = a.morton_index();
    auto morton_b = b.morton_index();

    // Creating the queue that will store the octants left to process.
    // Initially, we need to process all the children of a.get_closest_ancestor(b)
    std::queue<Octree<Dimension>> W;
    for (const auto & child : a.get_closest_ancestor(b).get_children())
        W.push(child);

    // And we loop while there is an octant to process
    while (!W.empty()) {
        Octree<Dimension> const & w = W.front();
        auto morton_w = w.morton_index();

        // If the current octant is between a and b
        if ((morton_w > morton_a) && (morton_w < morton_b) && (!w.is_ancestor(b)))
            completed_region.push_back(w);
        // Else if it's an ancestor of a or b
        else if (w.is_ancestor(a) || w.is_ancestor(b)) {
            for(const auto & child : w.get_children())
                W.push(child);
        }
        // Finally don't forget to remove the processed octant.
        W.pop();
    }

    return completed_region;
}


#endif //SWARMING_PROJECT_COMPLETE_REGION_H
