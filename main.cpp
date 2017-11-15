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

    //std::cout << grid << std::endl;
    std::vector<Boid <std::uniform_real_distribution<float>, 3> > vec;
    for(int i=0; i<3; i++){
        Boid<std::uniform_real_distribution<float>, 3> bob(bottom_left, top_right);
        bob.cohesion_update(vec);
        bob.separation_update(vec);
        bob.alignment_update(vec);
        bob.border_force_update(bottom_left, top_right);
        std::cout << bob << std::endl;
        vec.push_back(bob);
    }

    return 0;
}
