#ifndef SWARMING_PROJECT_CONSTANTS_H
#define SWARMING_PROJECT_CONSTANTS_H

namespace constants {

    constexpr const double DOUBLE_EPSILON{1e-10};

    constexpr const double PI{3.14159265};
    constexpr const float COHESION_NORMALISER = 0.01;
    constexpr const float ALIGNMENT_NORMALISER = static_cast<float>(1.0/8.0);
    constexpr const float SEPARATION_NORMALISER = 0.1;
    constexpr const float BORDER_SEPARATION_NORMALISER = 1.0;

    constexpr const float VISION_DISTANCE = 5.0;
    constexpr const float VISION_ANGLE = 90.0; //In degrees

    constexpr const float SEPARATION_MIN_DISTANCE = 1;
    constexpr const float REPULSION_DISTANCE = 1.0;
    constexpr const float BORDER_SEPARATION_MIN_DISTANCE = 7.5;

    constexpr const float MAX_SPEED = 1.0;

    constexpr const float TIMESTEP = 0.5;

}

#endif //SWARMING_PROJECT_CONSTANTS_H
