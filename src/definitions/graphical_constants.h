#ifndef SWARMING_PROJECT_GRAPHICAL_CONSTANTS_H_H
#define SWARMING_PROJECT_GRAPHICAL_CONSTANTS_H_H

#include <cstddef>

namespace gconst {

    constexpr const float       BOID_RADIUS_COEFFICIENT{0.001};
    constexpr const std::size_t BOID_NUMBER_OF_SIDES{10};
    constexpr const std::size_t VTK_COORDINATES_NUMBER{3};
    constexpr const std::size_t UPDATE_DELAY_MS{10};
    double BACKGROUND_COLOR[3] = {0.0, 0.0, 0.0};
    double BOID_COLOR[3] = {1.0, 1.0, 1.0};
    double BOUNDS_COLOR[3] = {0.7, 0.7, 0.7};

}
#endif //SWARMING_PROJECT_GRAPHICAL_CONSTANTS_H_H
