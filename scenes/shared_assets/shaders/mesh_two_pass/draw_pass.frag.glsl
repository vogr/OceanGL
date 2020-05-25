#version 330 core

in struct fragment_data
{
    vec4 position;
    vec4 normal;
    vec4 color;
    vec2 texture_uv;
    vec4 light_view_coords;
} fragment;

layout(location = 0) out vec4 FragColor;

uniform sampler2D object_texture_sampler;
uniform sampler2D caustics_texture_sampler;
//uniform sampler2D depths_in_light_ref_sampler;
uniform sampler2DShadow depths_in_light_ref_sampler;


uniform vec3 camera_position;
uniform vec3 light_position;
uniform float light_view_size;

uniform vec3 color     = vec3(1.0, 1.0, 1.0);
uniform float color_alpha = 1.0;
uniform float ambiant  = 0.1;
uniform float diffuse  = 0.8;
uniform float specular = 0.5;
uniform int specular_exponent = 128;


void main()
{
    // Phong shading
    vec3 n = normalize(fragment.normal.xyz);
    vec3 u = normalize(light_position-fragment.position.xyz);
    vec3 r = reflect(u,n);
    vec3 t = normalize(fragment.position.xyz-camera_position);


    float view_from_light = 0.;
    // Logic if `depths_in_light_ref_sampler` is a sampler2DShadow
    if (fragment.light_view_coords.w >= 0) {
        // Fragment is in view

        // Epsilon for depth test (on z) in pre-normalized coordinates
        vec4 eps = vec4(0,0,0.0000005,0) * fragment.light_view_coords.w;

        // Test if fragment.light_view_coords.z < depths_in_light_ref_sampler at coordinates fragment.light_view_coords.xy
        // (after normalization)
        view_from_light = textureProj(depths_in_light_ref_sampler, fragment.light_view_coords - eps);
        // returns value in [0,1] : 1 if true, 0 if false ; values in between are obtained by
        // interpolating on the neighborings coordinates
    }

    /*
    // Logic if `depths_in_light_ref_sampler` is a sampler2D
    float min_depth_from_light_source = textureProj(depths_in_light_ref_sampler, fragment.light_view_coords.xyw).r;
    if(min_depth_from_light_source +0.0000001 >= fragment.light_view_coords.z / fragment.light_view_coords.w) {
        view_from_light = 1.;
    }
    */

    // illumation takes into acount :
    // - if there is a direct line btw light source and object (with `view_from_light`, a value in [0,1])
    // - the color of the caustics animation at that point (with value `caustics_illumination`)

    // multiply coordinates by a factor to make the pattern smaller
    // fragment.light_view_coords.xy -> coords in light view in [0,1]
    // light_position.xy + light_view_size *  fragment.light_view_coords.xy -> w,y cords in world space
    float caustics_illumination = textureProj(caustics_texture_sampler, vec3((light_position.xy + light_view_size * fragment.light_view_coords.xy) / 40 , fragment.light_view_coords.w)).r;
    float illumination = view_from_light * 0.7 * caustics_illumination;

    float diffuse_value  = illumination * diffuse * clamp( dot(u,n), 0.0, 1.0);
    float specular_value = illumination * specular * pow( clamp( dot(r,t), 0.0, 1.0), specular_exponent);

    vec3 white = vec3(1.0);
    vec4 color_texture = texture(object_texture_sampler, fragment.texture_uv);
    vec3 c = (ambiant+diffuse_value)*color.rgb*fragment.color.rgb*color_texture.rgb + specular_value*white;

    // Add fog
    float dist_from_view = length(camera_position - fragment.position.xyz);
    float fog = 1 - 0.7 * exp(-0.01 * dist_from_view);

    c = (1 - fog) * c + fog * vec3(0.282, 0.239, 0.545);


    FragColor = vec4(c, color_texture.a*fragment.color.a*color_alpha);
}
