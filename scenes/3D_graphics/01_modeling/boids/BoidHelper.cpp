#include "BoidHelper.hpp"
#include <cmath>

BoidHelper::BoidHelper () {
    directions.resize(numViewDirections);

    float goldenRatio = (1.f + std::sqrt (5)) / 2;
    float angleIncrement = M_PI * 2 * goldenRatio;

    for (int i = 0; i < numViewDirections; i++) {
        float t = (float) i / numViewDirections;
        float inclination = std::acos (1 - 2 * t);
        float azimuth = angleIncrement * i;

        float x = std::sin(inclination) * std::cos(azimuth);
        float y = std::sin(inclination) * std::sin(azimuth);
        float z = std::cos(inclination);
        directions[i] = vcl::vec3(x, y, z);
    }
}
