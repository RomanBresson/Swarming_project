#include <iostream>
#include "data_structures/Boid.h"
#include "data_structures/Grid.h"
#include "definitions/types.h"

#include <random>

constexpr std::size_t DIMENSIONTEST = 2;
using types::Position;
using Vector3Pos = Position<DIMENSIONTEST>;


int main() {
    const int nb_of_boids = 50;
    Vector3Pos bottom_left({0.0, 0.0});
    Vector3Pos top_right({100.0, 100.0});
    Grid< std::uniform_int_distribution<int>, DIMENSIONTEST > grid( bottom_left, top_right, nb_of_boids );

    for(int t = 0; t<1000; t++){
        std::cout << grid << std::endl;
        for(int i=0; i<nb_of_boids; i++){
            std::vector<Boid<std::uniform_int_distribution<int>, DIMENSIONTEST> > neighbours;
            for(int j=0; j<nb_of_boids; j++){
                if (i!=j){
                    if (grid.m_boids[i].is_visible(grid.m_boids[j])){
                        neighbours.push_back(grid.m_boids[j]);
                    }
                }
            }
            grid.m_boids[i].update_forces(neighbours, bottom_left, top_right);
            grid.m_boids[i].update_velocity(neighbours);
            grid.m_boids[i].update_position(bottom_left, top_right);
            //Boid<std::uniform_real_distribution<float>, 3> bob(bottom_left, top_right);
            //bob.update_forces(vec, bottom_left, top_right);
            //bob.update_velocity(vec);
            //std::cout << bob << std::endl;
            //vec.push_back(bob);
        }
    }
    /*
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
