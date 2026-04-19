#pragma once

namespace galena {

constexpr float Epsilon = 0.0001f;

inline float Sign(float x) { return (x > 0) - (x < 0); }

inline float Clamp(float x, float a, float b)
{
    return (x < a ? a : (x > b ? b : x));
}

}  // namespace galena
