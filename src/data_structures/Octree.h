#ifndef SWARMING_PROJECT_OCTREE_H
#define SWARMING_PROJECT_OCTREE_H

#include "definitions/types.h"
#include "definitions/constants.h"
#include "data_structures/Boid.h"
#include <vector>

using types::Coordinate;

/**
* Class that represents an octree.
* @tparam m_depth      The depth of our octree
* @tparam Dimension    The dimension of the space (each tree has a maximum of 2**Dimension children).
*/
template <std::size_t Dimension>
class Octree {

public:
    int m_depth;
    Coordinate<Dimension> m_anchor;

    /**
    * Constructor for the Octree class.
    * @param depth  Depth of the octree, MUST BE STRICTLY INFERIOR TO DMAX.
    * @param anchor The number of randomly-distributed boids initially in the grid.
    */
    Octree(Coordinate<Dimension> const & anchor, int const & depth)
        : m_anchor(anchor),
        m_depth(depth)
    {
    }

    Octree(Boid<Dimension> const & boid)
    : m_depth(constants::Dmax)
    {
        for (int i=0; i<Dimension; i++){
            float case_size = GRID_SIZE/pow(2, constants::Dmax);
            m_anchor[i] = (int)(boid.m_position[i]/case_size);
        }
    }

    int morton_index(){
        int morton_enc = m_depth & 0x1F;
        int k = 5;
        for (int j=0; j<constants::Dmax; j++){
            for (int i=0; i<Dimension; i++){
                int x = m_anchor[i]>>j;
                morton_enc +=(x%2)*pow(2, k);
                k++;
            }
        }
        return(morton_enc);
    }

    bool is_child(Octree<Dimension> const & poss_father) const{
        if (m_depth != poss_father.m_depth+1){
            return false;
        }
        for (int i = 0; i<Dimension; i++){
            if ((m_anchor[i] != poss_father.m_anchor[i]) && (m_anchor[i] != poss_father.m_anchor[i]+pow(2, constants::Dmax-m_depth))){
                return false;
            }
        }
        return true;
    }

    int is_descendant(Octree<Dimension> const & poss_ances) const{
        if (m_depth <= poss_ances.m_depth){
            return 0;
        }
        for (int i = 0; i<Dimension; i++){
            if ((m_anchor[i] < poss_ances.m_anchor[i]) || (m_anchor[i] >= poss_ances.m_anchor[i]+pow(2, constants::Dmax-poss_ances.m_depth))){
                return 0;
            }
        }
        return m_depth-poss_ances.m_depth;
    }

    bool is_father(Octree<Dimension> const & poss_son){
        return poss_son.is_child(*this);
    }

    int is_ancestor(Octree<Dimension> const & poss_desc) const {
        return poss_desc.is_descendant(*this);
    }

    Octree<Dimension> get_father() const { //assertions on dimension ?
        Coordinate<Dimension> anchor = m_anchor;
        int case_size = pow(2, constants::Dmax-m_depth+1); //size of the father
        for(int i = 0; i<Dimension; i++){
            anchor[i] -= (anchor[i]%case_size);
        }
        Octree<Dimension> father(anchor, m_depth-1);
        return father;
    }

    Octree<Dimension> get_closest_ancestor(Octree<Dimension> b) const{ //assertions on order ?
        Octree<Dimension> curr_ances(m_anchor, m_depth);
        while (! curr_ances.is_ancestor(b)) {
            curr_ances = curr_ances.get_father();
        }
        return curr_ances;
    }

    std::vector<Octree<Dimension>> get_children() const{
        std::vector<Octree<Dimension>> children;
        for (int i = 0; i < pow(2, Dimension); i++) {
            Octree<Dimension> child(m_anchor, m_depth + 1);
            int currindex = i;
            int case_size = pow(2, Dmax-m_depth-1);
            for (int j = 0; j<Dimension; j++){
                child.m_anchor[j] += currindex%2*case_size;
                currindex/=2;
            }
            children.push_back(child);
        }
        return(children);
    }
};

#endif //SWARMING_PROJECT_OCTREE_H
