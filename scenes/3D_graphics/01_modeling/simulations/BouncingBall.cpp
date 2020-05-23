//
// Created by vogier on 06/05/2020.
//

#include "BouncingBall.h"

#include "../terrain/terrain.h"

using namespace  vcl;

void BouncingBall::move(float dt) {
  const vec3 F = mass * (*g);

  // Numerical integration (symplectic Euler)
  velocity = velocity + dt*F/mass;
  transform.translation = transform.translation + dt*velocity;

}

void BouncingBall::move_and_collide(float dt) {
  /* Hard-coded reliance on terrain function ;
   * ideally should take a list of possibly colliding objects
   */
  move(dt);

  vec2 const uv = terrain_xy_to_uv({transform.translation.x, transform.translation.y});
  auto terrain_elevation = evaluate_terrain_z(uv.x,uv.y);
  if(transform.translation.z - radius < terrain_elevation) {
    // Collision
    auto const N = terrain_normal(uv.x, uv.y);
    transform.translation.z = terrain_elevation + radius;
    auto const vn = dot(velocity, N);
    //float k1 = 0.9f, k2 = 0.7f;
    float k1 = 1.f, k2 = 1.f;
    velocity = k1 * velocity - (k1 + k2) * vn * N;
  }
}

void BouncingBall::update(float dt) {
  move_and_collide(dt);
  if (transform.translation.z < -30.f) {
    marked_for_deletion = true;
  }
}

void BouncingBall::draw(const vcl::camera_scene& camera, GLuint shader) {
  model->uniform.transform = transform;
  vcl::draw(*model, camera, shader);
}