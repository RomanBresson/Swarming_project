#ifndef SWARMING_PROJECT_OCTREE_H
#define SWARMING_PROJECT_OCTREE_H

#include "definitions/types.h"
#include "definitions/constants.h"
#include <array>

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
            if ((m_anchor[i] != poss_father.m_anchor[i]) && (m_anchor[i] != poss_father.m_anchor[i]+pow(2, Dmax-m_depth))){
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
            if ((m_anchor[i] < poss_ances.m_anchor[i]) || (m_anchor[i] >= poss_ances.m_anchor[i]+pow(2, Dmax-poss_ances.m_depth))){
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
};

#endif //SWARMING_PROJECT_OCTREE_H
