//
// Created by vogier on 20/04/2020.
//

#pragma once
#include "vcl/vcl.hpp"
#include "WorldElement.h"

#include "../trajectories/Trajectory.h"

class AnimatedBird : public WorldElement {
public:
  vcl::hierarchy_mesh_drawable hierarchy;
  vcl::timer_interval animation_timer; // defaults to [0., 1.]
  Trajectory trajectory;

  AnimatedBird() = default;
  AnimatedBird* clone() override {return new AnimatedBird(*this);}

  void build(std::map<std::string,GLuint>& shaders, vcl::buffer<keyframe> trajectory_keyframes);
  void draw(const vcl::camera_scene& camera, const vcl::light_animation_data & light_data, vcl::DrawType draw_type);

private:
  void update();
  void update_transform();
  void update_global_coordinates();
};


