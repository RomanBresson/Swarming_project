#ifndef SWARMING_PROJECT_GRAPHICAL_CONSTANTS_H_H
#define SWARMING_PROJECT_GRAPHICAL_CONSTANTS_H_H

#include <cstddef>

namespace gconst {

    constexpr const float BOID_RADIUS{0.01};
    constexpr const std::size_t BOID_NUMBER_OF_SIDES{10};
    constexpr const std::size_t VTK_COORDINATES_NUMBER{3};
    constexpr const std::size_t UPDATE_DELAY_MS{2000};

}
#endif //SWARMING_PROJECT_GRAPHICAL_CONSTANTS_H_H
