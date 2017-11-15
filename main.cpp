#include <iostream>
#include "data_structures/Boid.h"
#include "definitions/types.h"

using types::Vector3Pos;
using types::Vector3Vel;
using types::Vector3Force;

int main() {
    std::vector<Boid> vec;
    Boid bob(Vector3Pos({0.0, 0.0, 0.0}), Vector3Pos({1.0, 10.0, 100.0}));
    for(int i = 0; i<9;i++){
        Boid X(Vector3Pos({0.0, 0.0, 0.0}), Vector3Pos({1.0, 10.0, 100.0}));
        X.m_position = {i,-i,0};
        vec.push_back(X);
    }
    bob.m_position = bob.compute_center_of_mass(vec);
    std::cout << bob.toString() << std::endl;
    return 0;
}
