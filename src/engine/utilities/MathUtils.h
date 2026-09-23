#pragma once

namespace opendash::engine
{

inline float deg(float radians) {
    return radians * 57.29577951f; // Match CC_RADIANS_TO_DEGREES
}

inline float rad(float degrees) {
    return degrees * 0.01745329252f; // Match CC_DEGREES_TO_RADIANS
}

// TODO slerp2D, squareDistance, etc...
    
}