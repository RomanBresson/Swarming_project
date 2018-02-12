#ifndef SWARMING_PROJECT_LINEAR_OCTREE_H
#define SWARMING_PROJECT_LINEAR_OCTREE_H

#include "mpi.h"
#include "definitions/types.h"
#include "definitions/constants.h"
#include "data_structures/Octree.h"
#include "data_structures/MathArray.h" //REMOVE AFTER TESTS
#include <vector>
#include <queue>
#include <list>
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
    explicit Linear_Octree(std::vector<Octree<Dimension>> octants)
        : m_octants(octants)
    { }

    /**
    * Constructor for the Linear Octree class (algorithm 3).
    * @param a : first octant
    * @param b : last octant
    */
    Linear_Octree(Octree<Dimension> a, Octree<Dimension> b)
    {
        std::size_t morton_a = a.morton_index();
        std::size_t morton_b = b.morton_index();

        // Creating the queue that will store the octants left to process.
        // Initially, we need to process all the children of a.get_closest_ancestor(b)
        std::queue<Octree<Dimension>> W;
        for(const auto & child : a.get_closest_ancestor(b).get_children())
            W.push(child);

        // And we loop while there is an octant to process
        while (!W.empty()){
            Octree<Dimension> const & w = W.front();
            std::size_t morton_w = w.morton_index();

            // If the current octant is between a and b
            if((morton_w > morton_a) && (morton_w < morton_b) && (!w.is_ancestor(b)))
                m_octants.push_back(w);
            // Else if it's an ancestor of a or b
            else if (w.is_ancestor(a) || w.is_ancestor(b)) {
                std::vector<Octree<Dimension>> children = w.get_children();
                for(const auto & child : w.get_children())
                    W.push(child);
            }

            W.pop();
        }
    }

    /**
    * Constructor for the Linear Octree class (algorithm 4).
    * @param L : partial sorted list of octants
    */
    Linear_Octree(std::list<Octree<Dimension>> partial_list)
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

        if (process_ID == 0){
            partial_list.push_front(
                root.get_dfd()
                .get_closest_ancestor(partial_list.front())
                .get_children().front());
        }
        else if (process_ID == process_number-1){
            partial_list.push_back(partial_list.back()
                .get_closest_ancestor(root.get_dld())
                .get_children().back());
        }

        MPI_Request ignored_request;
        if (process_ID != 0){
            // TODO: False!!! Will be destroyed before Isend finish.
            std::array<int, Dimension+1> octree_msg;
            octree_msg[0] = partial_list.front().m_depth;
            std::copy(partial_list.front().m_anchor.begin(), partial_list.front().m_anchor.end(), std::next(octree_msg.begin()));
            MPI_Isend(octree_msg.data(), octree_msg.size(), MPI_INT, process_ID-1, 0, MPI_COMM_WORLD, & ignored_request);
        }

        if (process_ID < process_number-1){
            std::array<int,Dimension+1> octree_msg_rcvd;
            MPI_Recv(octree_msg_rcvd.data(), octree_msg_rcvd.size(), MPI_INT, process_ID+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            Coordinate<Dimension> rcv_anchor;
            std::copy(octree_msg_rcvd.begin(), std::prev(octree_msg_rcvd.end()), rcv_anchor.begin());
            Octree<Dimension> octree_received(rcv_anchor, octree_msg_rcvd[octree_msg_rcvd.size()-1]);
        }

        typename std::list<Octree<Dimension>>::iterator it;
        for(it = partial_list.begin(); it!=std::prev(partial_list.end(),1); ++it){
            Linear_Octree<Dimension> A(*it, *(std::next(it))); //Algo 3
            m_octants.push_back(*it);
            for (int j=0; j<A.m_octants.size(); j++){
                m_octants.push_back(A.m_octants[j]);
            }
        }

        if(process_ID == process_number-1){
            m_octants.push_back(partial_list.back());
        }
    }
    /**
    * Removes duplicates from the vector
    * The vector must be sorted
    */
    void remove_duplicates() {
        for (int i = 1; i < m_octants.size(); i++) {
            if ((m_octants.at(i-1).m_anchor == m_octants.at(i).m_anchor) && (m_octants.at(i-1).m_depth == m_octants.at(i).m_depth)) {
                m_octants.erase(std::next(m_octants.begin()+i-2));
                i--;
            }
        }
    }

    /**
    * Removes duplicates from the list
    * The list must be sorted
    */
    void remove_duplicates(std::list<Octree<Dimension>> & octants_list) {
        typename std::list<Octree<Dimension>>::iterator it;
        for(it = octants_list.begin(); it!=std::prev(octants_list.end(),1); ++it){
            if ((*it).morton_index() == (*(std::next(it, 1))).morton_index()){
                octants_list.erase(next(it,1));
                it--;
            }
        }
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
