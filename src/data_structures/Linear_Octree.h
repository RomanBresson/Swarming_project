#ifndef SWARMING_PROJECT_LINEAR_OCTREE_H
#define SWARMING_PROJECT_LINEAR_OCTREE_H

#include <vector>
#include <list>
#include <algorithm>
#include <iterator>
#include "mpi/sample_sort.h"

#include "definitions/types.h"
#include "definitions/constants.h"
#include "mpi.h"
#include "data_structures/Octree.h"

#include "algorithms/complete_region.h"
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
    {
        int process_number, process_ID;

        MPI_Comm_size(MPI_COMM_WORLD, &process_number);
        MPI_Comm_rank(MPI_COMM_WORLD, &process_ID);

        remove_duplicates(partial_list);

        Coordinate<Dimension> anchor_root;
        for (int i = 0; i<Dimension; i++) {
            anchor_root[i] = 0.0;
        }

        Octree<Dimension> root(anchor_root, 0);

        if (process_ID == 0) {
            partial_list.push_front(
                root.get_dfd()
                .get_closest_ancestor(partial_list.front())
                .get_children().front());
        }
        else if (process_ID == process_number-1) {
            partial_list.push_back(partial_list.back()
                .get_closest_ancestor(root.get_dld())
                .get_children().back());
        }

        MPI_Request ignored_request;
        std::array<int, Dimension+1> octree_msg;
        if (process_ID != 0) {
            octree_msg[0] = partial_list.front().m_depth;
            std::copy(partial_list.front().m_anchor.begin(), partial_list.front().m_anchor.end(), std::next(octree_msg.begin()));
            MPI_Isend(octree_msg.data(), octree_msg.size(), MPI_INT, process_ID-1, 0, MPI_COMM_WORLD, & ignored_request);
        }

        std::array<int,Dimension+1> octree_msg_rcvd;
        if (process_ID < process_number-1) {
            MPI_Recv(octree_msg_rcvd.data(), octree_msg_rcvd.size(), MPI_INT, process_ID+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            Coordinate<Dimension> rcv_anchor;
            std::copy(octree_msg_rcvd.begin(), std::prev(octree_msg_rcvd.end()), rcv_anchor.begin());
            Octree<Dimension> octree_received(rcv_anchor, octree_msg_rcvd[octree_msg_rcvd.size()-1]);
        }

        typename std::list<Octree<Dimension>>::iterator it;

        for(it = partial_list.begin(); it!=std::prev(partial_list.end(),1); ++it) {
            Linear_Octree<Dimension> A(*it, *(std::next(it))); //Algo 3
            m_octants.push_back(*it);
            for (int j=0; j<A.m_octants.size(); j++){
                m_octants.push_back(A.m_octants[j]);
            }
        }

        if(process_ID == process_number-1) {
            m_octants.push_back(partial_list.back());
        }
    }

};

#endif //SWARMING_PROJECT_LINEAR_OCTREE_H
