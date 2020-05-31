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