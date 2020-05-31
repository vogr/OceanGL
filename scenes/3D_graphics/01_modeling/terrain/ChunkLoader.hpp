#pragma once

#include "vcl/vcl.hpp"
#include <unordered_map>
#include <tuple>

#include "Chunk.hpp"

#include "../sharks_manager/SharksManager.hpp"


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
    typedef std::unordered_map<const chunk_id, Chunk, key_hash> map_t;

public:
    GLuint texture_id{0};
    int radius_to_load = 3; // Manhattan distance

    float shark_spawn_probability = 0.07;
    unsigned int n_billboards_per_chunk = 15;

    ChunkLoader() = default;
    explicit ChunkLoader(GLuint _texture_id, int _radius_to_load = 3);

    void load_chunk(int i, int j);
    void unload_chunk(int i, int j);
    bool is_chunk_loaded(int i, int j);

    void update_center(vcl::vec3 camera_position, SharksManager & shark_manager);

    void draw(const vcl::camera_scene &camera, const vcl::light_animation_data &light_data, vcl::DrawType draw_type);

    std::vector<std::reference_wrapper<WorldElement>> get_obstacles_in_chunk(int i, int j);

private:
    map_t loaded_chunks;
    std::tuple<int,int> center {0,0};

    // Keep a model for each type of billboard : each one is a texture
    // on a mesh. We make use of polymorphism, and therefore we need
    // to use pointers so as not to slice our objects
    std::vector<CrossBillboard> billboards_models;
    // List of references to pass to the chunk builder functions
    std::vector<std::reference_wrapper<CrossBillboard>> billboards_models_view;
};