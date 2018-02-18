#ifndef SWARMING_PROJECT_BLOCK_PARTITION_H
#define SWARMING_PROJECT_BLOCK_PARTITION_H

#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <array>

#include "mpi.h"
#include "definitions/constants.h"
#include "data_structures/Octree.h"
#include "algorithms/is_sorted_distributed.h"
#include "algorithms/complete_region.h"
#include "algorithms/complete_octree.h"
#include "algorithms/sorted_range_count_distributed.h"
#include "algorithms/partition.h"
#include "algorithms/merge_sorted_arrays.h"

#if SWARMING_DO_ALL_CHECKS == 1
#include <cassert>
#endif

template <std::size_t Dimension>
std::vector< Octree<Dimension> > block_partition(std::vector< Octree<Dimension> > & F) {

    std::vector< Octree<Dimension> > block_list;

    int process_number, process_ID;
    MPI_Comm_size(MPI_COMM_WORLD, &process_number);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_ID);

#if SWARMING_DO_ALL_CHECKS == 1
    assert(is_sorted_distributed(F));
#endif

    auto const T = complete_region(F.front(), F.back());

    // Find all the octants with the lowest level in T
    std::list< Octree<Dimension> > C;
    auto const lowest_level = std::min_element(T.begin(), T.end(),
                                               [](Octree<Dimension> const & lhs,
                                                  Octree<Dimension> const & rhs){ return lhs.m_depth < rhs.m_depth; })->m_depth;
    for(auto const & octant : T) {
        if(octant.m_depth == lowest_level)
            C.push_back(octant);
    }

    std::vector< Octree<Dimension> > G = complete_octree(C);


    // Update of weights

    //std::function<unsigned long long(Octree<Dimension> const &)> weight =
    //        [&G,&F](Octree<Dimension> const &g){
                // Number of octants in F (*globally*) that are descendant of g. All the descendant of g have a morton
                // index higher than g and lower than the deepest last descendant of g. See Appendix A, Property 10 of
                // the article.
                // This function will be called only once on each octant of G, so we don't need to implement a
                // kind of memoization to speed up the procedure.
    //            return sorted_range_count_distributed(F, g, g.get_dld());
    //        };

    //partition(G, weight);

    // Here G is still sorted and covers the whole space

    // Redistribution of F: each processor will ask for the octants in F covered by the octants they have in G.
    std::vector< std::vector< Octree<Dimension> > > received_sorted_data;
    MPI_Request request;
    for(std::size_t p{0}; p < process_number; ++p) {

        // Store the bounds of each processors.
        std::array< Octree<Dimension>, 2> bounds = {G.front(), G.back().get_dld()};
        // Broadcast the bounds from processor p.
        MPI_Bcast(bounds.data(), 2 * sizeof(Octree<Dimension>), MPI_BYTE, p, MPI_COMM_WORLD);

        // Then compute the octants to send to processor p from the broadcasted bounds.
        auto const first_element = std::lower_bound(F.begin(), F.end(), bounds[0]);
        auto const last_element  = std::upper_bound(F.begin(), F.end(), bounds[1]);
        // We know that std::distance will output a positive integer because first_element is before last_element.
        std::size_t const number_of_elements_to_send{ static_cast<std::size_t>(std::distance(first_element, last_element)) };

        // Then send the size of the data
        MPI_Isend(&number_of_elements_to_send, 1, MPI_UNSIGNED_LONG_LONG, p, /*tag*/ 0, MPI_COMM_WORLD, &request);
        // And send the data if needed
        if(number_of_elements_to_send > 0)
            MPI_Isend(&(*first_element), number_of_elements_to_send * sizeof(Octree<Dimension>), MPI_BYTE,
                      p, /*tag*/ 1, MPI_COMM_WORLD, &request);

        // And receive if we are the processor p
        if(process_ID == p) {
            std::size_t number_of_elements_to_receive;
            for(std::size_t proc{0}; proc < process_number; ++proc) {
                MPI_Recv(&number_of_elements_to_receive, 1, MPI_UNSIGNED_LONG_LONG, proc, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                received_sorted_data.emplace_back(number_of_elements_to_receive);
                if(number_of_elements_to_receive > 0)
                    MPI_Recv(received_sorted_data.back().data(), number_of_elements_to_receive * sizeof(Octree<Dimension>),
                             MPI_BYTE, proc, /*tag*/ 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
        // Wait for the sending operations to finish because we don't want to exit the scope and destroy the memory
        // locations that are sent.
        MPI_Wait(&request, MPI_STATUS_IGNORE);
    }

    // Now merge the received data
    F = merge_sorted_arrays_sequential(received_sorted_data);
    return G;
}

#endif //SWARMING_PROJECT_BLOCK_PARTITION_H
