#pragma once

#include "vcl/vcl.hpp"
#include <unordered_map>

#include "../models/WorldElement.h"

using namespace vcl;

float evaluate_terrain_z(float u, float v);
vec3 evaluate_terrain(float u, float v);
mesh_drawable create_chunk(GLuint texture, int i, int j);

vcl::vec2 terrain_xy_to_uv(vcl::vec2 xy);
vcl::vec2 terrain_uv_to_xy(vcl::vec2 uv);
vcl::vec3 terrain_normal(float u, float v);

vcl::vec3 get_available_position(std::vector<WorldElement> const &  elements, double min_radius);





class ChunkLoader {
private:
    // Define hash function for tuple<int,int> and make a custom unoredered_map using
    // this function :
    // https://stackoverflow.com/questions/20834838/using-tuple-in-unordered-map
    typedef std::tuple<int, int> chunk_id;
    struct key_hash : public std::unary_function<chunk_id, std::size_t> {
        std::size_t operator()(const chunk_id &k) const {
          return 31 * std::get<0>(k) + std::get<1>(k);
        }
    };
    typedef std::unordered_map<const chunk_id, mesh_drawable, key_hash> map_t;

public:
    GLuint texture_id{0};
    int radius_to_load = 3; // Manhattan distance

    ChunkLoader() = default;
    explicit ChunkLoader(GLuint _texture_id, int _radius_to_load = 3)
    : texture_id{_texture_id}, radius_to_load{_radius_to_load}
    { };

    void load_chunk(int i, int j);
    void unload_chunk(int i, int j);
    bool is_chunk_loaded(int i, int j);

    void update_center(vcl::vec3 camera_position);

    void draw(const vcl::camera_scene &camera, const vcl::light_animation_data &light_data, vcl::DrawType draw_type) const;

private:
    mesh_drawable create_chunk(int i, int j) const;
    map_t loaded_chunks;
    std::tuple<int,int> center {0,0};
};
