#include "Chunk.hpp"

#include "../models/billboards.h"

#include <algorithm>
#include <memory>

// Generate terrain mesh for chunk (i,j)
Chunk::Chunk(int i, int j, GLuint texture_id, std::vector<std::unique_ptr<WorldElement>> &billboards_models, unsigned int n_billboard_per_chunk)
: chunk_id{i,j}
{
  //TODO: Seed with deterministic int based on chunk_id

  // Number of samples of the terrain is N x N
  const size_t N = 25;

  mesh terrain_cpu; // temporary terrain storage (CPU only)
  terrain_cpu.position.resize(N*N);
  terrain_cpu.texture_uv.resize(N*N);

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
      terrain_cpu.position[kv+N*ku] = evaluate_terrain(u,v);
      terrain_cpu.texture_uv[kv+N*ku] = {tu, tv};
    }
  }
  terrain_cpu.texture_uv[0] = {0., 0.};


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

      terrain_cpu.connectivity.push_back(triangle_1);
      terrain_cpu.connectivity.push_back(triangle_2);
    }
  }

  terrain = mesh_drawable{std::move(terrain_cpu)};
  terrain.texture_id = texture_id;
  terrain.uniform.color = {0.6f,0.85f,0.5f};
  terrain.uniform.shading.specular = 0.0f; // non-specular terrain material

  populate_billboards(billboards_models, n_billboard_per_chunk);
}



void Chunk::draw(const vcl::camera_scene &camera, const vcl::light_animation_data &light_data, vcl::DrawType draw_type) {
  vcl::draw(terrain, camera, light_data, draw_type);
  // Sort the billboards from furthest to closest...

  vec3 const p = camera.camera_position();
  std::sort(billboards.begin(), billboards.end(),[&p](std::unique_ptr<WorldElement> const & e1, std::unique_ptr<WorldElement> const & e2) {
    vec3 const u1 = e1->transform.translation - p;
    vec3 const u2 = e2->transform.translation - p;
    float const d1_2 = dot(u1,u1);
    float const d2_2 = dot(u2,u2);
    return d1_2 > d2_2;
  }
  );



  // ... and draw them.
  for (auto & el : billboards) {
    el->draw(camera, light_data, draw_type);
  }
}


void Chunk::populate_billboards(std::vector<std::unique_ptr<WorldElement>> const & billboards_models, unsigned int n_billboard_per_chunk) {
  for (int i = 0; i < n_billboard_per_chunk; i++) {
    int itexture = static_cast<int>(std::floor(vcl::rand_interval(0, billboards_models.size() - 1)));
    auto &el_to_add = billboards_models[itexture];

    vec3 next_pos = get_available_position(billboards, el_to_add->radius);

    billboards.emplace_back(el_to_add->clone());
    billboards.back()->transform.translation += next_pos;
  }
}


vec3 Chunk::get_available_position(std::vector<std::unique_ptr<WorldElement>> const & elements, double min_radius) const {
  int const i = std::get<0>(chunk_id), j = std::get<1>(chunk_id);
  vec3 next_pos {};
  bool found = false;
  while(!found) {
    float u = i + rand_interval(0.f, 1.f);
    float v = j + rand_interval(0.f, 1.f);
    next_pos = evaluate_terrain(u,v);
    found = true;
    for(auto const & el : elements) {
      auto d = norm(vec3{el->transform.translation.x - next_pos.x, el->transform.translation.y - next_pos.y, 0.f});
      if(d <  min_radius + el->radius) {
        found = false;
        break;
      }
    }
  }
  return next_pos;
}