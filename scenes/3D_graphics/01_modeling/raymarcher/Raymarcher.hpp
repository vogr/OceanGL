#pragma once

#include "vcl/vcl.hpp"
#include "../models/WorldElement.h"

class Raymarcher {
public:
  int MAX_MARCHING_STEPS = 50;
  float EPSILON = 0.1;
  float min_depth = 0.;
  float max_depth = 50.;

  // Returns :
  // - ptr to closest obstacle in dir `direction` starting from point `starting_point` if such an obstacle exists
  //   betwween min_detph and max_depth
  // - nullptr if no such obstacle exists
  WorldElement* raymarch(vcl::vec3 starting_point, vcl::vec3 direction, std::vector<std::reference_wrapper<WorldElement>> const & obstacles_to_consider) const;
};


