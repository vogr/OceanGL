//
// Created by vogier on 13/05/2020.
//

#include "terrain.h"

#include <cmath>

vec2 terrain_xy_to_uv(vec2 xy) {
  const float u = xy.x / 100 + 0.5f;
  const float v =  xy.y / 100 + 0.5f;
  return {u, v};
}
vec2 terrain_uv_to_xy(vec2 uv) {
  const float x = 100*(uv.x-0.5f);
  const float y = 100*(uv.y-0.5f);
  return {x, y};
}



// Evaluate height of the terrain for any (u,v) \in [0,1]
float evaluate_terrain_z(float u, float v)
{
  std::vector<vec2> u0 {
          {0.5f, 0.5f},
          {0.3f, 0.8f},
          {0.3f, 0.8f},
          {0.8f, 0.3f},
          {0.1f, 0.2f},
          {0.1f, 0.8f},
          {0.8f, 0.8f}
  };
  std::vector<float> h {3.0f, 1.0f, -2.f, -2.f, 1.5f, -1.5f, 7.f};
  std::vector<float> sigma {0.9f, 0.2f, 0.8f, 0.3f, 0.24f, 15.f, 0.2f};
  float z = 0.;
  for(size_t i = 0 ; i < u0.size(); i++) {
    float const d = norm(vec2(u,v)-u0[i]) / sigma[i];
    z += h[i] * std::exp(-d*d);
  }

  const float scaling = 3.f;
  const int octave = 9;
  const float persistency = 0.5f;
  const float height = 3.7f;

  // Evaluate Perlin noise
  const float noise = perlin(scaling*u, scaling*v, octave, persistency);
  z += height * noise;
   return z;
}

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vec3 evaluate_terrain(float u, float v)
{
  const vec2 xy = terrain_uv_to_xy({u,v});
  const float z = evaluate_terrain_z(u,v);
  return {xy.x,xy.y,z};
}

vcl::vec3 terrain_normal(float u, float v) {
  /* Le plan tangent est paramétré par les vecteurs
   *    dS / dx
   *    dS / dy
   */

  float eps = 0.01f;

  auto M = evaluate_terrain(u,v);
  auto Mx = evaluate_terrain(u+eps, v);
  auto My = evaluate_terrain(u, v+eps);

  auto dxS = (Mx - M) / (Mx.x - M.x);
  auto dyS = (My - M) / (My.y - M.y);

  auto N = normalize(cross(dxS, dyS));
  return N;
}




