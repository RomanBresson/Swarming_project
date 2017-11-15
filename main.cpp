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
    int nb_of_boids = 10000;
    std::vector<Boid <std::uniform_real_distribution<float>, 3> > vec;
    for(int i=0; i<nb_of_boids; i++){
        Boid<std::uniform_real_distribution<float>, 3> bob(bottom_left, top_right);
        bob.update_forces(vec, bottom_left, top_right);
        bob.update_velocity(vec);
        std::cout << bob << std::endl;
        vec.push_back(bob);
    }
    for(int j = 0; j<1000; j++){
        for(int i=0; i<nb_of_boids; i++){
            vec[i].update_forces(vec, bottom_left, top_right);
            vec[i].update_velocity(vec);
            std::cout << vec[i] << std::endl;
        }
        std::cout << "-------------------------------" << std::endl;
    }
    return 0;
}
