//
// Created by vogier on 20/04/2020.
//

#include "AnimatedBird.h"
#include "models.h"

using namespace vcl;




void AnimatedBird::build(std::map<std::string,GLuint>& shaders, buffer<keyframe> trajectory_keyframes) {
  trajectory.init(std::move(trajectory_keyframes));

  // The geometry of the body is a sphere
  mesh_drawable body {mesh_primitive_sphere(0.3f, {0,0,0}, 40, 40)};
  body.uniform.transform.scaling_axis = {1.25f,1.f,2.1f};
  //body.texture_id = texture_gpu_white();

  mesh_drawable head {mesh_primitive_sphere(0.2f, {0,0,0}, 40, 40)};
  //head.texture_id = texture_gpu_white();

  // Geometry of the eyes: black spheres
  mesh_drawable eye {mesh_primitive_sphere(0.05f, {0,0,0}, 20, 20)};
  //eye.texture_id = texture_gpu_white();
  eye.uniform.color = {0,0,0};



  mesh wing_in_cpu;
  wing_in_cpu.position.push_back({0.f,0.f,0.f});
  wing_in_cpu.position.push_back({0.f,0.f,0.2f});
  wing_in_cpu.position.push_back({0.8f,0.f,0.2f});
  wing_in_cpu.position.push_back({0.f,0.f,0.8f});
  wing_in_cpu.position.push_back({0.8f,0.f,0.8f});
  wing_in_cpu.position.push_back({0.f,0.f,1.f});
  for (unsigned int i = 0; i < 4; i++) {
    wing_in_cpu.connectivity.push_back({i, i+1, i+2});
    wing_in_cpu.connectivity.push_back({i+2, i+1, i});
  }
  mesh_drawable wing_in {wing_in_cpu};
  //wing_in.texture_id = texture_gpu_white();
  wing_in.uniform.transform.scaling = 0.7f;


  mesh wing_out_cpu;
  wing_out_cpu.position.push_back({0.f,0.f,0.f});
  wing_out_cpu.position.push_back({0.3f,0.f,0.1f});
  wing_out_cpu.position.push_back({0.f,0.f,0.2f});
  wing_out_cpu.position.push_back({0.3f,0.f,0.3f});
  wing_out_cpu.position.push_back({0.f,0.f,0.4f});
  wing_out_cpu.position.push_back({0.3f,0.f,0.5f});
  wing_out_cpu.position.push_back({0.f,0.f,0.6f});
  for (unsigned int i = 0; i < 5; i += 2) {
    wing_out_cpu.connectivity.push_back({i, i+1, i+2});
    wing_out_cpu.connectivity.push_back({i+2, i+1, i});
  }
  mesh_drawable wing_out {wing_out_cpu};
  //wing_out.texture_id = texture_gpu_white();
  wing_out.uniform.transform.scaling = 0.7f;

  mesh_drawable beak {create_cone(0.05f, 0.2f,0.f)};
  //beak.texture_id = texture_gpu_white();
  beak.uniform.color = {0.8f,0.1f,0.f};
  beak.uniform.transform.rotation = rotation_from_axis_angle_mat3({1,0,0},0.4f);

  // Build the hierarchy:
  // Syntax to add element
  //   hierarchy.add(visual_element, element_name, parent_name, (opt)[translation, rotation])
  hierarchy.add(body, "body");

  hierarchy.add(head, "head", "body", {0.f, 0.15f, 0.62f});

  // Eyes positions are set with respect to some ratio of the
  hierarchy.add(eye, "eye_left", "head" , 0.2f * vec3( 1/3.0f, 8/20.0f, 1/1.3f));
  hierarchy.add(eye, "eye_right", "head", 0.2f * vec3(-1/3.0f, 8/20.0f, 1/1.3f));

  // beak
  hierarchy.add(beak, "beak", "head", {0.f,-0.05f,0.15f});

  //Add wings
  hierarchy.add(wing_in, "wing_in_left", "body", {0.19f,0.f,-.27f});
  hierarchy.add(wing_out, "wing_out_left", "wing_in_left", {0.7f * 0.8f,0.f,0.7f*.2f});

  hierarchy.add(wing_in, "wing_in_right", "body",
          {{-0.2f,0.f,-.3f}, {-1,0,0, 0,1,0, 0,0,1}});
  hierarchy.add(wing_out, "wing_out_right", "wing_in_right",
          {0.7f * 0.8f,0.f,0.7f*.2f});
  hierarchy.set_shader_for_all_elements(shaders["mesh"]);
}

void AnimatedBird::draw(const vcl::camera_scene& camera, const light_animation_data & light_data, DrawType draw_type) {
  update();
  vcl::draw(hierarchy, camera, light_data, draw_type);
}


void AnimatedBird::update() {
  update_transform();
  update_global_coordinates();
}

void AnimatedBird::update_global_coordinates() {
  animation_timer.update();
  float t = animation_timer.t;

  // move top node (supposedly body) to its global position.
  hierarchy.elements[0].transform = transform;

  mat3 const R_head = rotation_from_axis_angle_mat3({0,1,0}, static_cast<float>(0.15 * std::sin(2* M_PI *t)) );
  hierarchy["head"].transform.rotation = R_head;


  mat3 const R_wings_left = rotation_from_axis_angle_mat3({0,0,1}, static_cast<float>(0.5 * std::sin(4 * M_PI *t)) );
  hierarchy["wing_in_left"].transform.rotation = R_wings_left;
  hierarchy["wing_out_left"].transform.rotation = R_wings_left;

  mat3 sym {-1,0,0, 0,1,0, 0,0,1};
  //mat3 const R_wings_right = rotation_from_axis_angle_mat3({0,0,1}, static_cast<float>(0.5 * std::sin(4 * M_PI *t) + M_PI)  );
  hierarchy["wing_in_right"].transform.rotation = sym * R_wings_left;
  hierarchy["wing_out_right"].transform.rotation = R_wings_left;

  hierarchy.update_local_to_global_coordinates();
}

void AnimatedBird::update_transform() {
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
  mat3 R_bird {trajectory.normal, trajectory.binormal, trajectory.tangent};
  transform = {trajectory.position, R_bird};
}