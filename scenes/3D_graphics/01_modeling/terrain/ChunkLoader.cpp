#include "ChunkLoader.hpp"

#include "../models/billboards.h"

ChunkLoader::ChunkLoader(GLuint _texture_id, int _radius_to_load)
: texture_id{_texture_id}, radius_to_load{_radius_to_load}
{
  int const N_PLANTS = 24;
  billboards_models.reserve(N_PLANTS);
  billboards_models_view.reserve(N_PLANTS);
  std::string const root = "scenes/3D_graphics/01_modeling/assets/ocean_plants/ocean_plant";
  std::string const ext = ".png";

  // All billboards will share the same meshes definition in the VBO,
  // only the texture ID will change.
  CrossBillboardMesh cross{0};
  //FlatBillboardMesh flat{0};

  std::vector<float> t_scale {16, 20, 23, 21, 21, 15, 12, 15, 20, 23, 19, 27, 29, 14, 25, 23, 20, 28, 17, 13, 28, 25, 24};
  std::vector<float> z_correction(N_PLANTS, -2.);
  z_correction[3]  = -7;
  z_correction[12] = -4;
  z_correction[15] = -4;
  z_correction[16] = -4;
  z_correction[17] = -8;
  z_correction[20] = -2.5;


  for (int i = 0; i < N_PLANTS; i++) {
    char id[3];
    sprintf(id, "%02d", i);
     auto tex_id = create_texture_gpu(
            image_load_png(root + id + ext),
            GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER
    );
     float scale = t_scale[i];
     float radius = 1.;
    {
      // The initial transform should place the object at coordinates (0,0),
      // it will then be translated to its final position in the chunks
      // by a simple translation
      cross.set_texture_id(tex_id);
      billboards_models.emplace_back(cross, affine_transform{{0,0,z_correction[i]}, {}, scale}, radius);
      billboards_models_view.push_back(std::ref(billboards_models.back()));
    }
    /*
    {
      // The initial transform should place the object at coordinates (0,0),
      // it will then be translated to its final position in the chunks
      // by a simple translation
      flat.billboard.texture_id = tex_id;
      auto w = std::make_unique<FlatBillboard>(flat, affine_transform{{0,0,-2}, {}, scale}, radius);
      billboards_models.push_back(std::move(w));
    }
    */
  }
}

void ChunkLoader::load_chunk(int i, int j) {
  loaded_chunks.emplace(std::make_tuple(i,j), Chunk{i,j, texture_id, billboards_models_view, n_billboards_per_chunk});
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
