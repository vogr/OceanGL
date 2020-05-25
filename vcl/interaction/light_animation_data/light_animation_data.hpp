#pragma once

#include <vcl/interaction/camera/camera.hpp>
#include <vcl/wrapper/glad/glad.hpp>

namespace vcl {
    enum class DrawType {
        STANDARD, PASS0, PASS1
    };

    class light_animation_data {
    public:
        light_animation_data() = default;
        light_animation_data(GLuint shader0, GLuint shader1);
        mat4 orthographic_projection_matrix() const;


        // /!\ We use a camera but
        camera_scene light_camera;


        GLuint shader_pass0{0};
        GLuint shader_pass1{0};

        float view_size = 800.;
        float z_near = 0.01f;
        float z_far = 500.f;

        // framebuffer object : will be used as rendering target in first
        // pass (off-line rendering). It will contain a simple texture `depth`
        GLuint light_view_fbo{0};

        // Handle to the texture stored in light_view_fbo. This texture
        // will be filled by the first pass, and read from at the second pass.
        // When viewed from the the light source point of view, a fragment at position (x,y) has
        // a depth z. This texture stores the depth of the fragment closest to the light source
        // at position (x,y) : i.e. if we determine that a fragment (when viewed by the user) is
        // at this depth (when viewed by the light source), then we know that it is directly illuminated
        // by the light source (no obstruction), and therefore we can draw a caustics animation.
        // WARNING : as a texture, it should nbe indexed by values in [0,1].
        // (x,y) in [-1,1] needs to be mapped to a value in [0,1]
        GLuint depth_texture_id{0};

        // Id to the caustices texture that will be used to draw.
        // Should be changed on each iteration.
        GLuint caustics_sprite_id{0};
    };
}