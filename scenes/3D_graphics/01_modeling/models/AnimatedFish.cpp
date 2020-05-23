//
// Created by vogier on 20/04/2020.
//

#include "AnimatedFish.h"
#include "models.h"

using namespace vcl;



void AnimatedFish::draw(const vcl::camera_scene& camera, const light_animation_data & light_data, DrawType draw_type) {
  model.uniform.transform = transform;
  vcl::draw(model, camera, light_data, draw_type);
}

void AnimatedFish::update() {
  update_transform();
  //update animation : wiggle tail ?;
}

void AnimatedFish::update_transform() {
  trajectory.update();
  // Draw animated models

  /*
  // projection de la tangente sur (xOy)
  vec3 T0 {trajectory.tangent};
  T0.z = 0.f;
  // construction de TNB
  vec3 T = normalize(T0);
  vec3 B = {0,0,1};
  vec3 N = normalize(cross(B, T));
  //envoie repère de l'oiseau dans TNB, z sur T.
  mat3 R_bird {N, B, T};
  */

  // use TBF frame (using quaternion slerp)
  mat3 R {trajectory.normal, trajectory.binormal, trajectory.tangent};
  transform = {trajectory.position, R};
}