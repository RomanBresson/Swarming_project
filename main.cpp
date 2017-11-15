#include <iostream>
#include "data_structures/Boid.h"
#include "data_structures/Grid.h"
#include "definitions/types.h"

#include <random>

using types::Position;
using Vector3Pos = Position<3>;

int main() {
    Vector3Pos bottom_left({0.0, 0.0, 0.0});
    Vector3Pos top_right({10.0, 10.0, 10.0});
    Grid< std::uniform_real_distribution<float>, 3 > grid( bottom_left, top_right, 10 );

    std::cout << grid << std::endl;

    return 0;
}
