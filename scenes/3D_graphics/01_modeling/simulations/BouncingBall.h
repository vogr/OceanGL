//
// Created by vogier on 06/05/2020.
//

#pragma once
#include <memory>

#include "vcl/vcl.hpp"
#include "../models/WorldElement.h"

class BouncingBall : public WorldElement {
public:
  vcl::vec3 velocity;
  std::shared_ptr<vcl::mesh_drawable> model;
  double mass {1.0};
  std::shared_ptr<vcl::vec3> g;

  BouncingBall(vcl::vec3 p_, vcl::vec3 v_, std::shared_ptr<vcl::mesh_drawable> m_, std::shared_ptr<vcl::vec3> g_, double r_)
  : WorldElement{{p_}, r_}, velocity{v_}, model{std::move(m_)}, g{std::move(g_)}
  { transform.scaling = r_; };

  bool marked_for_deletion {false};

  void move(float dt);
  void move_and_collide(float dt);
  void update(float dt);

  void draw(const vcl::camera_scene& camera, GLuint shader) override;
};


