#include "Chunk.hpp"


#include <algorithm>
#include <memory>

// Generate terrain mesh for chunk (i,j)
Chunk::Chunk(int i, int j, GLuint texture_id, std::vector<std::reference_wrapper<CrossBillboard>> const &billboards_models, unsigned int n_billboard_per_chunk)
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
      // Compute local parametric coordinates (u,v) \in [i,i+1] x [j, j+1]
      const float u = i + ku/(N-1.0f);
      const float v = j + kv/(N-1.0f);

      // Compute texture coordinates : use repetition (multiply by factor > 1).
      // Probably best to keep an integer factor.
      float const tv = u * 5;
      float const tu = v * 5;

      // Compute coordinates
      terrain_cpu.position[kv+N*ku] = evaluate_terrain(u,v);
      terrain_cpu.texture_uv[kv+N*ku] = {tu, tv};
    }
  }

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
  std::sort(billboards.begin(), billboards.end(),[&p](std::unique_ptr<DrawableWorldElement> const & e1, std::unique_ptr<DrawableWorldElement> const & e2) {
    vec3 const u1 = e1->getPosition() - p;
    vec3 const u2 = e2->getPosition() - p;
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


void Chunk::populate_billboards(std::vector<std::reference_wrapper<CrossBillboard>> const &billboards_models, unsigned int n_billboard_per_chunk) {
  // Ugly hack ! We'd like to pass the std::vector<std::unique_ptr<DrawableWorldElement>>& as a std::vector<std::unique_ptr<WorldElement>>&
  std::vector<std::reference_wrapper<WorldElement>> refs;
  for(auto b : billboards_models) {
    refs.emplace_back(b);
  }
  for (int i = 0; i < n_billboard_per_chunk; i++) {
    int itexture = static_cast<int>(std::floor(vcl::rand_interval(0, billboards_models.size() - 1)));
    auto el_to_add = billboards_models[itexture].get().clone();

    el_to_add->transform.translation += get_available_position(refs, el_to_add->radius);
    el_to_add->transform.rotation = vcl::rotation_from_axis_angle_mat3({0,0,1}, rand_interval(0.f, M_PI / 2));

    billboards.emplace_back(el_to_add);
    // Keep z_correction
  }
}


vec3 Chunk::get_available_position(std::vector<std::reference_wrapper<WorldElement>> const & elements, double min_radius) const {
  int const i = std::get<0>(chunk_id), j = std::get<1>(chunk_id);
  vec3 next_pos {};
  bool found = false;
  while(!found) {
    float u = i + rand_interval(0.f, 1.f);
    float v = j + rand_interval(0.f, 1.f);
    next_pos = evaluate_terrain(u,v);
    found = true;
    for(auto el_wrap : elements) {
      auto & el = el_wrap.get();
      auto p = el.getPosition();
      auto d = norm(vec3{p.x - next_pos.x, p.y - next_pos.y, 0.f});
      if(d <  min_radius + el.radius) {
        found = false;
        break;
      }
    }
  }
  return next_pos;
}