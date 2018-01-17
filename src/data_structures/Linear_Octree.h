#ifndef SWARMING_PROJECT_LINEAR_OCTREE_H
#define SWARMING_PROJECT_LINEAR_OCTREE_H

#include "definitions/types.h"
#include "definitions/constants.h"
#include "data_structures/Octree.h"
#include "data_structures/MathArray.h" //REMOVE AFTER TESTS
#include <vector>
#include <queue>
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
    * Constructor for the Linear Octree class (algorithm 3).
    * @param a : first octant
    * @param b : last octant
    */
    Linear_Octree(Octree<Dimension> a, Octree<Dimension> b)
    {
        int morton_a = a.morton_index();
        int morton_b = b.morton_index();
        std::cout << a.get_closest_ancestor(b).m_depth << " ----- " << a.get_closest_ancestor(b).m_anchor << std::endl;
        std::queue<Octree<Dimension>> W = a.get_closest_ancestor(b).get_children(); //Check a<b
        while (!W.empty()){
            Octree<Dimension> w = W.front();
            int morton_w = w.morton_index();
            if((morton_w > morton_a) && (morton_w < morton_b) && (!w.is_ancestor(b))){
                m_octants.push_back(w);
            } else if (w.is_ancestor(a) || w.is_ancestor(b)) {
                std::queue<Octree<Dimension>> children = w.get_children();
                while(!children.empty()){
                    Octree<Dimension> child = children.front();
                    W.push(child);
                    children.pop();
                }
            }
            W.pop();
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
