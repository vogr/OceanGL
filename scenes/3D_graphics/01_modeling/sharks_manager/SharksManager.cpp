//
// Created by vogier on 31/05/2020.
//

#include "SharksManager.hpp"

#include "../terrain/terrain.h"


SharksManager::SharksManager(mesh_drawable _shark_model)
: shark_model{_shark_model}
{ }


void SharksManager::spawn_shark_in_chunk(int i, int j) {
  float u = vcl::rand_interval(i, i+1);
  float v = vcl::rand_interval(j, j+1);
  auto terrain_xyz = evaluate_terrain(u, v);
  all_sharks.push_back(make_shark_at_position(terrain_xyz));
}

AnimatedFish SharksManager::make_shark_at_position(vcl::vec3 terrain_xyz) {
  AnimatedFish shark = AnimatedFish(shark_model);
  shark.radius = 10;

  size_t N_KEYFRAMES = 40;

  vcl::vec3 position = terrain_xyz + vcl::vec3{0,0, vcl::rand_interval(10,50)};
  float shark_speed = vcl::rand_interval(20.f, 30.f);
  float radius = vcl::rand_interval(70.f, 200.f);
  float z_range = vcl::rand_interval(0.f, 20.f);
  bool clockwise = (vcl::rand_interval() < 0.5f);

  shark.trajectory.init(make_random_shark_keyframes_from_starting_point(position, radius, shark_speed, z_range, clockwise, N_KEYFRAMES));
  shark.trajectory.timer.t = shark.trajectory.timer.t_min;
  return shark;
}



vcl::buffer<keyframe> SharksManager::make_random_shark_keyframes_from_starting_point(vcl::vec3 starting_point, float radius, float speed, float z_range, bool clockwise, int n_keyframes) {
  assert(n_keyframes >= 2);

  // Keyframes should end up like this for a closed trajectory :
  //         0                1           2                 N_K          N_K + 1      N_K + 2
  // [keyframe N_K - 1] [keyframe 0] [keyframe 1] ... [keyframe N_K-1] [keyframe 0] [keyframe 1]
  vcl::buffer<keyframe> trajectory_keyframes;

  vcl::vec3 center = starting_point + vcl::vec3{-1, 0, 0} * radius;

  int d = (clockwise ? -1 : 1);

  // dt = length on one arc / speed
  float dt = std::floor(2.f * M_PI * radius / (n_keyframes * speed));
  dt = 1.f;

  float t = 0.;
  for (int i = -1; i < n_keyframes + 2; i++) {
    auto i_f = static_cast<float>(i) * d;
    auto co = static_cast<float>(std::cos(2. * M_PI * i_f / n_keyframes));
    auto si = static_cast<float>(std::sin(2. * M_PI * i_f / n_keyframes));

    float x = center.x + radius * co;
    float y = center.y + radius * si;
    float z = center.z + z_range * static_cast<float>(std::abs(std::sin(M_PI * i_f / n_keyframes)));
    trajectory_keyframes.push_back({{x, y, z}, t});
    t += dt;
  }
  return trajectory_keyframes;
}


void SharksManager::update_all_sharks() {
  for (auto & shark : all_sharks) {
    shark.update();
  }
}


void SharksManager::draw_all_sharks(const vcl::camera_scene& camera, const vcl::light_animation_data & ca_data, vcl::DrawType draw_type) {
  for (auto & shark : all_sharks) {
    shark.draw(camera, ca_data, draw_type);
  }
}


std::vector<std::reference_wrapper<WorldElement>> SharksManager::get_shark_refs() {
  std::vector<std::reference_wrapper<WorldElement>> shark_refs;
  for(auto & shark : all_sharks) {
    shark_refs.emplace_back(shark);
  }
  return shark_refs;
}
