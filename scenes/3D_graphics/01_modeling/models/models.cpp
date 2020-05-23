//
// Created by vogier on 01/04/2020.
//
#include "vcl/vcl.hpp"
#include "models.h"

using namespace vcl;


mesh create_cylinder(float radius, float height) {
  unsigned int const N = 15;
  mesh cylinder;


  // We will store 2 * N points:
  //  * N on the top circle
  //  * N on the bottom circle
  cylinder.position.resize(2 * N);
  for (size_t ku = 0; ku < N; ku++) {
    auto const fcos = static_cast<float>(std::cos(2.f * M_PI * ku / N));
    auto const fsin = static_cast<float>(std::sin(2.f * M_PI * ku / N));
    float const x = radius * fcos;
    float const y = radius * fsin;
    cylinder.position[ku] = {x, y, 0.f};
    cylinder.position[ku + N] = {x, y, height};
  }

  // Define connectivity
  for (unsigned int i0 = 0; i0 < N ; i0++) {
    unsigned int const i1 = (i0 + 1) % N;
    unsigned int const i2 = i1 + N;
    unsigned int const i3 = i0 + N;
    cylinder.connectivity.push_back({i0, i1, i2});
    cylinder.connectivity.push_back({i2, i3, i0});
  }
  return cylinder;
}


mesh create_cone(float radius, float height, float z_offset) {
  mesh cone;
  const unsigned int N = 15;

  for (size_t ku = 0; ku < N; ku++) {
    auto const fcos = static_cast<float>(std::cos(2.f * M_PI * ku / N));
    auto const fsin = static_cast<float>(std::sin(2.f * M_PI * ku / N));
    float const x = radius * fcos;
    float const y = radius * fsin;
    cone.position.push_back({x, y, z_offset});
  }

  cone.position.push_back({0.f, 0.f, z_offset});
  cone.position.push_back({0.f, 0.f, z_offset + height});

  for (unsigned int i0 = 0; i0 < N; i0++) {
    unsigned int const i1 = (i0 + 1) % N;
    // Assign correct orientation for culling
    cone.connectivity.push_back({i1, i0, N});
    cone.connectivity.push_back({i0, i1, (N+1)});
  }
  return cone;
}


mesh create_tree_foliage(float radius, float height, float z_offset)
{
  mesh m = create_cone(radius, height, 0);
  m.push_back( create_cone(radius, height, z_offset) );
  m.push_back( create_cone(radius, height, 2*z_offset) );
  return m;
}

hierarchy_mesh_drawable TreeElement::create_tree_model(GLuint texture) {
  hierarchy_mesh_drawable model;

  mesh_drawable trunk {create_cylinder(0.1f, 1.f)};
  trunk.uniform.color = vec3{82.f, 60.f, 58.f} / 255;
  trunk.uniform.shading.specular = 0.05f;
  trunk.texture_id = texture;
  model.add(trunk, "trunk", "global_frame", {0.f,0.f,-0.25f});

  mesh_drawable foliage {create_tree_foliage(0.3f, .5f, 0.125f)};
  foliage.uniform.color = {0.f, 0.7f, 0.1f};
  foliage.uniform.shading.specular = 0.1f;
  foliage.texture_id = texture;

  model.add(foliage, "foliage", "trunk", {0.f, 0.f, 0.75f});
  return model;
}



void TreeElement::draw(const camera_scene& camera, const light_animation_data & light_data, DrawType draw_type) {
  tree->elements[0].transform = transform;
  tree->update_local_to_global_coordinates();
  vcl::draw(*tree, camera, light_data, draw_type);
}

MushroomMesh::MushroomMesh(GLuint texture)
: foot{create_cylinder(0.025f, 0.07f)},
  head{create_cone(0.045f, 0.025f, 0.05f)}
{
  foot.uniform.color = vec3{214.f, 208.f, 208.f} / 255.f;
  foot.texture_id = texture;

  head.uniform.color = vec3{176.f, 52.f, 40.f} / 255.f;
  head.texture_id = texture;
}

void MushroomElement::draw(const camera_scene& camera, GLuint shader) {
  mushroom.foot.uniform.transform.translation = transform.translation + vec3{0.f, 0.f, -0.015f};
  mushroom.head.uniform.transform.translation = transform.translation;
  vcl::draw(mushroom.foot, camera, shader);
  vcl::draw(mushroom.head, camera, shader);
}

