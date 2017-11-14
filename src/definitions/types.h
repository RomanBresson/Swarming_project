#ifndef SWARMING_PROJECT_TYPES_H
#define SWARMING_PROJECT_TYPES_H

#include <array>

using PositionType = float;
using Vector3Pos   = std::array<PositionType, 3>;

using VelocityType = float;
using Vector3Vel   = std::array<VelocityType, 3>;

using ForceType    = float;
using Vector3Force = std::array<ForceType, 3>;

#endif //SWARMING_PROJECT_TYPES_H
