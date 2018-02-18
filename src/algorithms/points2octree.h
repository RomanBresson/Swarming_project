#ifndef SWARMING_PROJECT_POINTS2OCTREE_H
#define SWARMING_PROJECT_POINTS2OCTREE_H

#include <vector>
#include <algorithm>
#include <list>

#include "definitions/constants.h"
#include "data_structures/Octree.h"
#include "data_structures/Boid.h"
#include "algorithms/block_partition.h"
#include "algorithms/sorted_range_count_distributed.h"
#include "algorithms/sample_sort.h"

template <std::size_t Dimension>
std::vector< Octree<Dimension> > points2octree(std::vector< Boid<Dimension> > const & boids,
                                               std::size_t Np_max) {

    int process_ID, process_number;

    MPI_Comm_size(MPI_COMM_WORLD, &process_number);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_ID);


    // Creating the octants at the deepest level possible.
    std::vector< Octree<Dimension> > F;
    F.reserve(boids.size());
    for(auto const & boid : boids)
        F.emplace_back(boid);

    // Sorting the created octants
    sample_sort_inplace(F);


    // Partition blocks using BlockPartition algorithm.
    std::vector< Octree<Dimension> > B = block_partition(F);

    // Refining blocks until there are no more than Np_max boids per octant.
    // Here we use a list because we don't want our vector to re-allocate & copy at each erase or push_back.
    std::list< Octree<Dimension> > B_list(B.begin(), B.end());
    auto it = B_list.begin();
    while(it != B_list.end()) {
        // Compute the number of boids covered by this octant.
        // TODO: problem here.
        std::size_t const number_of_points{ sorted_range_count_distributed(F, *it, it->get_dld()) };

        // If this number is too high then split the octant.
        if(number_of_points > Np_max) {
            auto const children = it->get_children();
            B_list.insert(B_list.end(), children.begin(), children.end());
            auto const previous = std::prev(it);
            B_list.erase(it);
            it = previous;
        }
        // Go to the next octant and loop.
        ++it;
    }

    // Return the result as a vector.
    return std::vector< Octree<Dimension> >(B_list.begin(), B_list.end());
}

#endif //SWARMING_PROJECT_POINTS2OCTREE_H
