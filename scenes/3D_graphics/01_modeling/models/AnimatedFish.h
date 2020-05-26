//
// Created by vogier on 20/04/2020.
//

#pragma once
#include "vcl/vcl.hpp"
#include "WorldElement.h"

#include "../trajectories/Trajectory.h"

class AnimatedFish : public WorldElement {
public:
  vcl::mesh_drawable model;
  Trajectory trajectory; // trajectory includes its own timer

  AnimatedFish() = default;
  AnimatedFish* clone() override {return new AnimatedFish(*this);}

  explicit AnimatedFish(vcl::mesh_drawable model)
  : model(std::move(model))
  { };

  void draw(const vcl::camera_scene& camera, const vcl::light_animation_data & ca_data, vcl::DrawType draw_type) override;
  void update();

private:
  void update_transform();
};


