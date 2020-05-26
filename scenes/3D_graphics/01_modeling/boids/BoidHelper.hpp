#pragma once

#include "vcl/vcl.hpp"
#include <vector>

class BoidHelper {
public:
    const int numViewDirections = 300;
    std::vector<vcl::vec3> directions;

    BoidHelper ();
};
