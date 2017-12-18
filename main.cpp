#include <iostream>
#include "data_structures/Boid.h"
#include "data_structures/Grid.h"
#include "visualization/GridVisualizer.h"

constexpr const std::size_t DIMENSION{3};
constexpr const std::size_t NUMBER_OF_BOIDS{500};
using types::Position;

int main() {
    Position<DIMENSION> bottom_left({0.0, 0.0, 0.0});
    Position<DIMENSION> top_right({100.0, 100.0, 100.0});

    Grid<std::uniform_real_distribution<float>, DIMENSION> grid(bottom_left, top_right, NUMBER_OF_BOIDS);
    GridVisualizer<std::uniform_real_distribution<float>, DIMENSION> visualizer(grid);

    visualizer.start();

    return 0;
}
