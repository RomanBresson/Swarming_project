#ifndef SWARMING_PROJECT_CONSTANTS_H
#define SWARMING_PROJECT_CONSTANTS_H

namespace constants {

    constexpr const double DOUBLE_EPSILON{1e-10};

    constexpr const double PI{3.14159265};
    constexpr const float COHESION_NORMALISER = 0.1;
    constexpr const float ALIGNMENT_NORMALISER = static_cast<float>(1.0/4.0);
    constexpr const float SEPARATION_NORMALISER = 2;
    constexpr const float BORDER_SEPARATION_NORMALISER = 1.0;

    constexpr const float VISION_DISTANCE = 5.0;
    constexpr const float VISION_ANGLE = 120.0; //In degrees

    constexpr const float SEPARATION_MIN_DISTANCE = 1;
    constexpr const float REPULSION_DISTANCE = 1.0;
    constexpr const float BORDER_SEPARATION_MIN_DISTANCE = 7.5;
    constexpr const std::size_t GRID_SIZE{100};

    constexpr const float MAX_SPEED = 1.0;

    constexpr const float TIMESTEP = 0.5;
    constexpr const int   Dmax = 3;
}

#endif //SWARMING_PROJECT_CONSTANTS_H
