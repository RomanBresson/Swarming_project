#ifndef SWARMING_PROJECT_OCTREE_H
#define SWARMING_PROJECT_OCTREE_H

#include "definitions/types.h"
#include "definitions/constants.h"
#include "data_structures/Boid.h"
#include "algorithms/sample_sort.h"
#include "algorithms/morton_index.h"



using types::Coordinate;
using types::CoordinateType;
/**
* Class that represents an octree.
* @tparam m_depth      The depth of our octree
* @tparam Dimension    The dimension of the space (each tree has a maximum of 2**Dimension children).
*/
template <std::size_t Dimension>
class Octree {

public:
    std::size_t m_depth{};
    Coordinate<Dimension> m_anchor;

    Octree() = default;

    /**
    * Constructor for the Octree class.
    * @param depth  Depth of the octree, MUST BE STRICTLY INFERIOR TO DMAX.
    * @param anchor The number of randomly-distributed boids initially in the grid.
    */
    Octree(Coordinate<Dimension> const & anchor, std::size_t const & depth)
            : m_anchor(anchor),
              m_depth(depth)
    { }

    /**
    * Constructor for the Octree class.
    * @param boid Boid that will spawn the octree at depth Dmax.
    */
    explicit Octree(Boid<Dimension> const & boid)
            : m_depth(constants::Dmax)
    {
        double const case_size{static_cast<double>(GRID_SIZE) / static_cast<double>(1 << constants::Dmax)};
        // TODO: we can use vectorisation here if we define the static_cast (or the cast) operation on a vector.
        for (std::size_t i{0}; i < Dimension; ++i)
            m_anchor[i] = static_cast<int>(boid.m_position[i] / case_size);
    }

    /**
    * Computes the morton index.
    */
    std::size_t morton_index() const{
        return get_morton_index(m_anchor, m_depth);
    }

    /**
    * Returns true if current octree is a child of the argument octree, false otherwise.
    * @param poss_father Possible father
    */
    bool is_child(Octree<Dimension> const & poss_father) const{
        if (m_depth != poss_father.m_depth+1){
            return false;
        }
        for (int i = 0; i<Dimension; i++){
            if ((m_anchor[i] != poss_father.m_anchor[i]) && (m_anchor[i] != poss_father.m_anchor[i]+(1<<(constants::Dmax-m_depth)))){
                return false;
            }
        }
        return true;
    }

    /**
    * Returns true if current octree is a is a descendant of the argument octree, false otherwise.
    * @param poss_ancestor Possible ancestor
    */
    int is_descendant(Octree<Dimension> const & poss_ances) const{
        if (m_depth <= poss_ances.m_depth){
            return 0;
        }
        for (int i = 0; i<Dimension; i++){
            if ((m_anchor[i] < poss_ances.m_anchor[i]) || (m_anchor[i] >= poss_ances.m_anchor[i]+(1<<(constants::Dmax-poss_ances.m_depth)))){
                return 0;
            }
        }
        return static_cast<int>(m_depth) - static_cast<int>(poss_ances.m_depth);
    }

    /**
    * Returns true if current octree is the father of the argument octree, false otherwise.
    * @param poss_son Possible son
    */
    bool is_father(Octree<Dimension> const & poss_son){
        return poss_son.is_child(*this);
    }

    /**
    * Returns true if current octree is an ancestor of the argument octree, false otherwise.
    * @param poss_ancestor Possible descendant
    */
    int is_ancestor(Octree<Dimension> const & poss_desc) const {
        return poss_desc.is_descendant(*this);
    }

    /**
    * Returns the father of the current octree.
    */
    Octree<Dimension> get_father() const {
#ifdef SWARMING_DO_ALL_CHECKS
        if (m_depth == 0){
            std::cerr << "WARNING: requesting father of a node at depth 0" << std::endl;
        }
#endif
        Coordinate<Dimension> anchor = m_anchor;
        const int case_size_minus_1 = (1 << (constants::Dmax-m_depth+1)) - 1;
        for(int i = 0; i<Dimension; i++){
            anchor[i] -= (anchor[i] & case_size_minus_1);
        }
        Octree<Dimension> father(anchor, m_depth-1);
        return father;
    }

    /**
    * Returns the closest ancestor shared by the current octant and the argument octant.
    * @param b Second Boid
    */
    Octree<Dimension> get_closest_ancestor(Octree<Dimension> b) const{
#ifdef SWARMING_DO_ALL_CHECKS
        if (*this > b){
            std::cerr << "WARNING: bad octant order" << std::endl;
        }
#endif
        Octree<Dimension> curr_ances(m_anchor, m_depth);
        while (! curr_ances.is_ancestor(b)) {
            curr_ances = curr_ances.get_father();
        }
        return curr_ances;
    }

    /**
    * Returns the vector containing every children of the current octree.
    */
    std::vector<Octree<Dimension>> get_children() const{
        // TODO: check that the returned children are ordered!
#ifdef SWARMING_DO_ALL_CHECKS
        if (m_depth == Dmax){
            std::cerr << "WARNING: Requesting children of a node at depth Dmax" << std::endl;
        }
#endif
        std::vector<Octree<Dimension>> children;
        children.reserve(1ULL << Dimension);

        for (std::size_t i{0}; i < (1ULL << Dimension); ++i) {
            Octree<Dimension> child(m_anchor, m_depth + 1);

            std::size_t case_size = (1ULL << (Dmax-m_depth-1));
            for (std::size_t j{0}; j < Dimension; ++j){
                child.m_anchor[j] += ((i >> j) & 1)*case_size;
            }
            children.push_back(child);
        }
        return(children);
    }

    Octree<Dimension> get_dfd() const{
        Octree<Dimension> dfd(m_anchor, constants::Dmax);
        return dfd;
    }

    Octree<Dimension> get_dld() const{
        Coordinate<Dimension> anchor = m_anchor;
        int case_size = (1<<(constants::Dmax-m_depth));
        for (int k=0; k<Dimension; k++){
            anchor[k] += case_size - 1;
        }
        Octree<Dimension> dld(anchor, constants::Dmax);
        return(dld);
    }

    std::vector<Octree<Dimension>> get_siblings() const {
        std::vector<Octree<Dimension>> siblings;
        if(m_depth == 0) return siblings;
        for(auto possible_sibling : this->get_father().get_children()) {
            if(possible_sibling != *this) {
                siblings.push_back(std::move(possible_sibling));
            }
        }
        return siblings;
    }

};

template <std::size_t Dimension>
bool operator==(const Octree<Dimension> & oct1, const Octree<Dimension> & oct2) {
    return oct1.m_depth == oct2.m_depth && oct1.m_anchor == oct2.m_anchor;
}

template <std::size_t Dimension>
bool operator!=(const Octree<Dimension> & oct1, const Octree<Dimension> & oct2) {
    return oct1.m_depth != oct2.m_depth || oct1.m_anchor != oct2.m_anchor;
}

template <std::size_t Dimension>
bool operator<(const Octree<Dimension> & oct1, const Octree<Dimension> & oct2) {
    return(oct1.morton_index() < oct2.morton_index());
};

template <std::size_t Dimension>
bool operator>(const Octree<Dimension> & oct1, const Octree<Dimension> & oct2) {
    return(oct1.morton_index() > oct2.morton_index());
};

template <std::size_t D>
std::ostream & operator<<(std::ostream & os, Octree<D> const & octree) {
    return os << "{ depth = " << octree.m_depth << ", anchor = " << octree.m_anchor << "}";
}

/**
 * Redefinition of numeric_limits<Octree<Dim>>::max() for the sort algorithm.
 */
namespace std {
    template<>
    template<std::size_t Dim>
    class numeric_limits<Octree<Dim>> {
    public:
        static Octree<Dim> max() {
            Coordinate<Dim> anchor;
            for(std::size_t i{0}; i < Dim; ++i) anchor[i] = static_cast<CoordinateType>(-1);
            return Octree<Dim>(anchor, static_cast<CoordinateType>(-1));
        }
    };
}

#endif //SWARMING_PROJECT_OCTREE_H
