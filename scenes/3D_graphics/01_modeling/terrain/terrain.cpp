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


// Generate terrain mesh for chunk (i,j)
mesh_drawable ChunkLoader::create_chunk(int i, int j) const
{
  // Number of samples of the terrain is N x N
  const size_t N = 25;

  mesh terrain; // temporary terrain storage (CPU only)
  terrain.position.resize(N*N);
  terrain.texture_uv.resize(N*N);

  // Fill terrain geometry
  for(size_t ku=0; ku<N; ++ku)
  {
    for(size_t kv=0; kv<N; ++kv)
    {
      // Compute local parametric coordinates (u,v) \in [0,1]
      const float u = i + ku/(N-1.0f);
      const float v = j + kv/(N-1.0f);

      // Compute texture coordinates : use repetition (values > 1.)
      float const tv = (N-1) * u * 0.2f;
      float const tu = (N-1) * v * 0.2f;


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
  terrain_gpu.texture_id = texture_id;
  terrain_gpu.uniform.color = {0.6f,0.85f,0.5f};
  terrain_gpu.uniform.shading.specular = 0.0f; // non-specular terrain material
  return terrain_gpu;
}

void ChunkLoader::load_chunk(int i, int j) {
  loaded_chunks[{i, j}] = create_chunk(i,j);
}

void ChunkLoader::unload_chunk(int i, int j) {
  loaded_chunks.erase({i,j});
}

bool ChunkLoader::is_chunk_loaded(int i, int j) {
  return (loaded_chunks.find({i,j}) != loaded_chunks.end());
}

void ChunkLoader::update_center(vcl::vec3 camera_position) {

  vec2 uv = terrain_xy_to_uv({camera_position.x, camera_position.y});

  int i = static_cast<int>(std::floor(uv.x));
  int j = static_cast<int>(std::floor(uv.y));

  center = {i,j};

  for (int s = -radius_to_load + 1; s < radius_to_load; s++) {
    for (int t = - (radius_to_load - 1 - std::abs(s)) ; abs(s)  + abs(t) < radius_to_load; t++) {
      int k = i + s, l = j + t;
      if(! is_chunk_loaded(k,l)) {
        load_chunk(k,l);
      }
    }
  }
}

void ChunkLoader::draw(const vcl::camera_scene &camera, const vcl::light_animation_data &light_data, vcl::DrawType draw_type) const {
  int i = std::get<0>(center), j = std::get<1>(center);

  for (int s = -radius_to_load + 1; s < radius_to_load; s++) {
    for (int t = - (radius_to_load - 1 - std::abs(s)) ; abs(s)  + abs(t) < radius_to_load; t++) {
      int k = i + s, l = j + t;
      vcl::draw(loaded_chunks.at({k,l}), camera, light_data, draw_type);
    }
  }
}