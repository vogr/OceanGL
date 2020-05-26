#include "ChunkLoader.hpp"

#include "../models/billboards.h"

ChunkLoader::ChunkLoader(GLuint _texture_id, int _radius_to_load)
: texture_id{_texture_id}, radius_to_load{_radius_to_load}
{
  billboards_models.reserve(2 * 9);
  // Load the 32 sprites of the the caustics animation
  std::string const root = "scenes/3D_graphics/01_modeling/assets/ocean_plants/ocean_plant";
  std::string const ext = ".png";

  // All billboards will share the same meshes definition int the VBO,
  // only the texture ID will change.
  CrossBillboardMesh cross{0};
  FlatBillboardMesh flat{0};

  std::vector<float> t_scale {15, 12, 20, 18, 10, 12, 30, 9};
  std::vector<float> t_radius {2, 1, 2, 2,   0.5, 0.5, 2, 1};

  for (int i = 0; i < 8; i++) {
    char id[2];
    sprintf(id, "%01d", i);
     auto tex_id = create_texture_gpu(
            image_load_png(root + id + ext),
            GL_REPEAT, GL_REPEAT
    );
     float scale = t_scale[i];
     float radius = t_radius[i];
    {
      // The initial transform should place the object at coordinates (0,0),
      // it will then be translated to its final position in the chunks
      // by a simple translation
      cross.set_texture_id(tex_id);
      auto w = std::make_unique<CrossBillboard>(cross, affine_transform{{0,0,-2}, {}, scale}, radius);
      billboards_models.push_back(std::move(w));
    }
    {
      // The initial transform should place the object at coordinates (0,0),
      // it will then be translated to its final position in the chunks
      // by a simple translation
      flat.billboard.texture_id = tex_id;
      auto w = std::make_unique<FlatBillboard>(flat, affine_transform{{0,0,-2}, {}, scale}, radius);
      billboards_models.push_back(std::move(w));
    }
  }
}

void ChunkLoader::load_chunk(int i, int j) {
  loaded_chunks.emplace(std::make_tuple(i,j), Chunk{i,j, texture_id, billboards_models, n_billboards_per_chunk});
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

void ChunkLoader::draw(const vcl::camera_scene &camera, const vcl::light_animation_data &light_data, vcl::DrawType draw_type) {
  int i = std::get<0>(center), j = std::get<1>(center);

  /* draw from furthest to closest */
  for (int d = radius_to_load - 1; d >= 0; d--) {
    for (int s = -d; s <= d; s++ ) {
      int t = d - abs(s);
      {
        int k = i + s, l = j + t;
        loaded_chunks.at({k, l}).draw(camera, light_data, draw_type);
      }
      if (t != 0) {
        int k = i + s, l = j - t;
        loaded_chunks.at({k, l}).draw(camera, light_data, draw_type);
      }
    }
  }
}