#include <iostream>
#include "data_structures/Boid.h"
#include "data_structures/Grid.h"
#include "definitions/types.h"
#include "visualization/GridVisualizer.h"

constexpr const std::size_t Dimension{2};

using types::Position;

int main() {
    Position<Dimension> bottom_left({0.0, 0.0});
    Position<Dimension> top_right({10.0, 10.0});

    Grid< std::uniform_real_distribution<float>, Dimension > grid( bottom_left, top_right, 100 );
    GridVisualizer<std::uniform_real_distribution<float>, Dimension> visualizer(grid);

    visualizer.start();

    return 0;
}
