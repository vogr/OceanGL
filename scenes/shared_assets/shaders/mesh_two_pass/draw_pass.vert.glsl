#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 texture_uv;

out struct fragment_data
{
    vec4 position;
    vec4 normal;
    vec4 color;
    vec2 texture_uv;
    vec4 light_view_coords;
} fragment;


// model transformation
uniform vec3 translation = vec3(0.0, 0.0, 0.0);                      // user defined translation
uniform mat3 rotation = mat3(1.0,0.0,0.0, 0.0,1.0,0.0, 0.0,0.0,1.0); // user defined rotation
uniform float scaling = 1.0;                                         // user defined scaling
uniform vec3 scaling_axis = vec3(1.0,1.0,1.0);                       // user defined scaling

// view transform
uniform mat4 view;
// perspective matrix
uniform mat4 perspective;


// light view transform
uniform mat4 light_view;
// perspective matrix
uniform mat4 light_perspective;

// [-1,1] to [0,1]
mat4 bias_matrix = mat4(
    0.5, 0, 0, 0, // 1st col
    0, 0.5, 0, 0, // 2nd col
    0, 0, 0.5, 0,
    0.5, 0.5, 0.5, 1
);

void main()
{
    // scaling matrix
    mat4 S = mat4(scaling*scaling_axis.x,0.0,0.0,0.0, 0.0,scaling*scaling_axis.y,0.0,0.0, 0.0,0.0,scaling*scaling_axis.z,0.0, 0.0,0.0,0.0,1.0);
    // 4x4 rotation matrix
    mat4 R = mat4(rotation);
    // 4D translation
    vec4 T = vec4(translation,0.0);


    fragment.color = color;
    fragment.texture_uv = texture_uv;

    fragment.normal = R*normal;
    vec4 position_transformed = R*S*position + T;

    // position in world space
    fragment.position = position_transformed;

    // position in screen space
    gl_Position = perspective * view * position_transformed;

    // position in screen space when viewed from light source
    // ie in [0,1] so that they can be used to
    // sample from a texture
    fragment.light_view_coords = bias_matrix * light_perspective * light_view * position_transformed;

}
