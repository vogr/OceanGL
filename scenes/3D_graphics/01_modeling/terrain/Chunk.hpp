#pragma once

#include "terrain.h"
#include "../models/billboards.h"

/**
 * Object representing a small part of the terrain : chunk (i,j)
 * contains the data necessary to draw the terrain for uv coordinates
 * in [i,i+1]x[j,j+1].
 * Stores the mesh representing the ground (terrain) and the billboards
 * on the ground.
 */

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
