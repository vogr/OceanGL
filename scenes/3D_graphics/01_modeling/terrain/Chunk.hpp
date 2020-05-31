#pragma once

#include "terrain.h"
#include "../models/billboards.h"

class Chunk {
public:
    Chunk(int i, int j, GLuint texture_id, std::vector<std::reference_wrapper<CrossBillboard>> const &billboards_models, unsigned int n_billboard_per_chunk=15);
    void draw(const vcl::camera_scene &camera, const vcl::light_animation_data &light_data, vcl::DrawType draw_type);

    std::tuple<int, int> chunk_id;
    mesh_drawable terrain;

    // Elements should be very lightweight, because they will be sorted
    // At the same time, the chunk should keep ownership of the billboards
    std::vector<std::unique_ptr<DrawableWorldElement>> billboards;

private:
    void populate_billboards(std::vector<std::reference_wrapper<CrossBillboard>> const &billboards_models, unsigned int n_billboard_per_chunk);
    vcl::vec3 get_available_position(std::vector<std::reference_wrapper<WorldElement>> const & elements, double min_radius) const;
};
