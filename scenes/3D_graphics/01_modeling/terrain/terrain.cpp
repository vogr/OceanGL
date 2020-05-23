//
// Created by vogier on 13/05/2020.
//

#include "terrain.h"


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
  if (u < 0 || u > 1 || v < 0 || v > 1) {
    return - std::numeric_limits<float>::infinity();
  }
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

// Generate terrain mesh
mesh_drawable create_terrain(GLuint texture)
{
  // Number of samples of the terrain is N x N
  const size_t N = 200;

  mesh terrain; // temporary terrain storage (CPU only)
  terrain.position.resize(N*N);
  terrain.texture_uv.resize(N*N);

  // Fill terrain geometry
  for(size_t ku=0; ku<N; ++ku)
  {
    for(size_t kv=0; kv<N; ++kv)
    {
      // Compute local parametric coordinates (u,v) \in [0,1]
      const float u = ku/(N-1.0f);
      const float v = kv/(N-1.0f);

      // Compute texture coordinates : use repetition (values > 1.)
      float const tv = kv * 0.2f;
      float const tu = ku * 0.2f;


      // Compute coordinates
      terrain.position[kv+N*ku] = evaluate_terrain(u,v);
      terrain.texture_uv[kv+N*ku] = {tu, tv};
    }
  }
  terrain.texture_uv[0] = {0., 0.};


  // Generate triangle organization
  //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
  const unsigned int Ns = N;
  for(unsigned int ku=0; ku<Ns-1; ++ku)
  {
    for(unsigned int kv=0; kv<Ns-1; ++kv)
    {
      const unsigned int idx = kv + N*ku; // current vertex offset

      const uint3 triangle_1 = {idx, idx+1+Ns, idx+1};
      const uint3 triangle_2 = {idx, idx+Ns, idx+1+Ns};

      terrain.connectivity.push_back(triangle_1);
      terrain.connectivity.push_back(triangle_2);
    }
  }

  mesh_drawable terrain_gpu {std::move(terrain)};
  terrain_gpu.texture_id = texture;
  terrain_gpu.uniform.color = {0.6f,0.85f,0.5f};
  terrain_gpu.uniform.shading.specular = 0.0f; // non-specular terrain material
  return terrain_gpu;
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

vec3 get_available_position(std::vector<WorldElement> const & elements, double min_radius) {
  vec3 next_pos {};
  bool found = false;
  while(!found) {
    float u = rand_interval(0.f, 1.f);
    float v = rand_interval(0.f, 1.f);
    next_pos = evaluate_terrain(u,v);
    found = true;
    for(auto const & el : elements) {
      auto d = norm(vec3{el.transform.translation.x - next_pos.x, el.transform.translation.y - next_pos.y, 0.f});
      if(d <  min_radius + el.radius) {
        found = false;
        break;
      }
    }
  }
  return next_pos;
}