#ifndef SWARMING_PROJECT_TYPES_H
#define SWARMING_PROJECT_TYPES_H

#include <array>

namespace types {

    using Position = float;
    using Vector3Pos   = std::array<Position, 3>;

    using Velocity = float;
    using Vector3Vel   = std::array<Velocity, 3>;

    using Force    = float;
    using Vector3Force = std::array<Force, 3>;

}
#endif //SWARMING_PROJECT_TYPES_H
