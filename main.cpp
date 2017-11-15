#include <iostream>
#include "data_structures/Boid.h"
#include "definitions/types.h"

using types::Vector3Pos;
using types::Vector3Vel;
using types::Vector3Force;

int main() {
    Boid bob(Vector3Pos({0.0, 0.0, 0.0}), Vector3Pos({1, 10, 100}));
    std::cout << bob.toString() << std::endl;
    return 0;
}
