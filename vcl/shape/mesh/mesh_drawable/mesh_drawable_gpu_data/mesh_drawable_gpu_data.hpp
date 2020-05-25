#pragma once


#include "vcl/wrapper/glad/glad.hpp"
#include "../../mesh_structure/mesh.hpp"


namespace vcl
{
struct mesh_drawable_gpu_data {

    mesh_drawable_gpu_data() = default;
    ~mesh_drawable_gpu_data();
    mesh_drawable_gpu_data(mesh_drawable_gpu_data const &) = delete;
    mesh_drawable_gpu_data& operator=(mesh_drawable_gpu_data const &) = delete;
    mesh_drawable_gpu_data(mesh_drawable_gpu_data &&) noexcept;
    mesh_drawable_gpu_data& operator=(mesh_drawable_gpu_data &&) noexcept;

    explicit mesh_drawable_gpu_data(mesh mesh_cpu);

    /** Clear buffers */
    void clear();

    /** Dynamically update the VBO with the new vector of position
     * Warning: new_position is expected to have the same size (or less) than the initialized one */
    void update_position(const buffer<vec3>& new_position);

    /** Dynamically update the VBO with the new vector of normal
     * Warning: new_normal is expected to have the same size (or less) than the initialized one */
    void update_normal(const buffer<vec3>& new_normal);


    GLuint vao {0};
    unsigned int number_triangles {0};

    GLuint vbo_index {0};      // Triplet (i,j,k) of triangle index

    GLuint vbo_position {0};   // (x,y,z) coordinates
    GLuint vbo_normal {0};     // (nx,ny,nz) normals coordinates (unit length)
    GLuint vbo_color {0};      // (r,g,b) values
    GLuint vbo_texture_uv {0}; // (u,v) texture coordinates
};

/** Call raw OpenGL draw */
void draw(const mesh_drawable_gpu_data& gpu_data);

}
