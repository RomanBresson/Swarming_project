#include <iostream>
#include "data_structures/Octree.h"
#include "data_structures/Boid.h"
#include "data_structures/Grid.h"
#include "visualization/GridVisualizer.h"

constexpr const std::size_t DIMENSION{3};
constexpr const std::size_t NUMBER_OF_BOIDS{500};
using types::Position;

int main() {
    Position<DIMENSION> bottom_left({0.0, 0.0, 0.0});
    Position<DIMENSION> top_right({GRID_SIZE, GRID_SIZE, GRID_SIZE});
    /*
    Position<DIMENSION> pos1({10.0,10.0,10.0});
    Position<DIMENSION> pos2({20.0,10.0,20.0});
    Position<DIMENSION> pos3({51.0,12.5,12.6});
    Velocity<DIMENSION> vel({0.0,0.0,0.0});
    Force<DIMENSION> forc({0.0,0.0,0.0});

    Boid<std::uniform_real_distribution<float>, DIMENSION> b1(pos1, vel, forc);
    Boid<std::uniform_real_distribution<float>, DIMENSION> b2(pos2, vel, forc);
    Boid<std::uniform_real_distribution<float>, DIMENSION> b3(pos3, vel, forc);

    Octree<DIMENSION> o1(b1);
    Octree<DIMENSION> o2(b2);
    Octree<DIMENSION> o3(b3);

    cout << o1.m_anchor << " ; " << o2.m_anchor <<" ; "<<o3.m_anchor<<std::endl;*/

    Grid<std::uniform_real_distribution<float>, DIMENSION> grid(bottom_left, top_right, NUMBER_OF_BOIDS);
    GridVisualizer<std::uniform_real_distribution<float>, DIMENSION> visualizer(grid);

    visualizer.start();

    return 0;
}
