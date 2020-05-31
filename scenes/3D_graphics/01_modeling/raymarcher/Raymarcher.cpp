#include "Raymarcher.hpp"


WorldElement * Raymarcher::raymarch(vcl::vec3 starting_point, vcl::vec3 direction, std::vector<std::reference_wrapper<WorldElement>> const & obstacles_to_consider) const {
  if (obstacles_to_consider.empty()) {
    return nullptr;
  }

  float depth = min_depth;

  for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
    vcl::vec3 p = starting_point + depth * direction;
    WorldElement * closest_obstacle = &obstacles_to_consider[0].get();
    float dist_to_closest = closest_obstacle->signed_distance(p);
    for(WorldElement & obstacle : obstacles_to_consider) {
      float d = obstacle.signed_distance(p);
      if (d < dist_to_closest) {
        closest_obstacle = &obstacle;
        dist_to_closest = d;
      }
    }
    if (dist_to_closest < EPSILON) {
      return closest_obstacle;
    }
    depth += dist_to_closest;
    if (depth >= max_depth) {
      return nullptr;
    }
  }
  return nullptr;
}