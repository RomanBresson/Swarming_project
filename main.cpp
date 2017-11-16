#include <iostream>
#include "data_structures/Boid.h"
#include "data_structures/Grid.h"
#include "definitions/types.h"

#include <random>

using types::Position;
using Vector3Pos = Position<2>;

int main() {
    const int nb_of_boids = 5;
    Vector3Pos bottom_left({0.0, 0.0});
    Vector3Pos top_right({10.0, 10.0});
    Grid< std::uniform_int_distribution<int>, 2 > grid( bottom_left, top_right, nb_of_boids );

    std::cout << grid << std::endl;
    for(int i=0; i<nb_of_boids; i++){
        for(int j=0; j<nb_of_boids; j++){
            if (i!=j){
                if (grid.m_boids[i].is_visible(grid.m_boids[j])){
                    std::cout << grid.m_boids[i] << "   CAN SEE   " << grid.m_boids[j] << std::endl;
                } else {
                    std::cout << grid.m_boids[i] << "   CANNOT SEE   " << grid.m_boids[j] << std::endl;
                }
            }
        }
        //Boid<std::uniform_real_distribution<float>, 3> bob(bottom_left, top_right);
        //bob.update_forces(vec, bottom_left, top_right);
        //bob.update_velocity(vec);
        //std::cout << bob << std::endl;
        //vec.push_back(bob);
    }/*
    for(int j = 0; j<1000; j++){
        for(int i=0; i<nb_of_boids; i++){
            vec[i].update_forces(vec, bottom_left, top_right);
            vec[i].update_velocity(vec);
            //std::cout << vec[i] << std::endl;
        }
        //std::cout << "-------------------------------" << std::endl;
    }*/
    return 0;
}
