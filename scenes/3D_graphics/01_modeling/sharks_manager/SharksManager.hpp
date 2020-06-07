#pragma once

#include "vcl/vcl.hpp"
#include "../models/AnimatedFish.h"

class SharksManager {
private:
  vcl::mesh_drawable shark_model;

  std::vector<AnimatedFish> all_sharks;
  //std::vector<std::reference_wrapper<WorldElement>> shark_refs;

  vcl::buffer<keyframe> make_random_shark_keyframes_from_starting_point(vcl::vec3 starting_point, float radius, float z_range, bool clockwise, int n_keyframes);
  AnimatedFish make_shark_at_position(vcl::vec3 position);
public:
  SharksManager() = default;
  explicit SharksManager(vcl::mesh_drawable _shark_model);
  void spawn_shark_in_chunk(int i, int j);
  void update_all_sharks();
  void draw_all_sharks(const vcl::camera_scene& camera, const vcl::light_animation_data & ca_data, vcl::DrawType draw_type);

  std::vector<std::reference_wrapper<WorldElement>> get_shark_refs();

};


