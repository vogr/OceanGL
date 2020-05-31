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



// Evaluate height of the terrain for any (u,v) \in R
float evaluate_terrain_z(float u, float v)
{
  const float scaling = 2.f;
  const int octave = 3;
  const float persistency = 0.6f;
  const float height = 4.5f;

  // Evaluate Perlin noise
  return height * perlin(scaling*u, scaling*v, octave, persistency);
}

// Evaluate 3D position of the terrain for any (u,v) \in R
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




