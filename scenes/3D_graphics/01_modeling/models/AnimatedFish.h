//
// Created by vogier on 20/04/2020.
//

#pragma once
#include "vcl/vcl.hpp"
#include "WorldElement.h"

#include "../trajectories/Trajectory.h"

class AnimatedFish : public DrawableWorldElement {
public:
  vcl::mesh_drawable model;
  Trajectory trajectory; // trajectory includes its own timer

  AnimatedFish() = default;
  AnimatedFish* clone() override {return new AnimatedFish(*this);}

  explicit AnimatedFish(vcl::mesh_drawable model)
  : model(std::move(model))
  { };

  void draw(const vcl::camera_scene& camera, const vcl::light_animation_data & ca_data, vcl::DrawType draw_type) override;
    vcl::vec3 getPosition() const override {return model.uniform.transform.translation; };
    void setPosition(vcl::vec3 p) override {model.uniform.transform.translation = p; };
    void update();

private:
    void update_transform();
};


