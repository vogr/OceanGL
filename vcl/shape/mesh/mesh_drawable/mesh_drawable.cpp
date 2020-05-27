#include "mesh_drawable.hpp"

#include "vcl/opengl/opengl.hpp"

namespace vcl
{

void draw_pass0(const mesh_drawable& drawable, light_animation_data const & light_data);
void draw_pass1(const mesh_drawable& drawable, const camera_scene& user_camera, light_animation_data const & light_data);


mesh_drawable::mesh_drawable(mesh mesh_arg, GLuint shader_arg, GLuint texture_id_arg)
    :data(std::make_shared<mesh_drawable_gpu_data>(std::move(mesh_arg))),uniform(),shader(shader_arg),texture_id(texture_id_arg)
{}

void mesh_drawable::clear()
{
    data->clear();
}

void mesh_drawable::update_position(const vcl::buffer<vec3>& new_position)
{
    data->update_position(new_position);
}

void mesh_drawable::update_normal(const vcl::buffer<vec3>& new_normal)
{
    data->update_normal(new_normal);
}

/*
 * Common setup for one-pass draw and two-pass draw.
 */
bool prepare_draw(const mesh_drawable& drawable, GLuint shader, GLuint fbo)
{
  // If shader is, skip display
  if(shader==0)
    return false;

  // Check that the shader is a valid one
  if( glIsProgram(shader)==GL_FALSE ) {
    std::cout<<"No valid shader set to display mesh: skip display"<<std::endl;
    return false;
  }

  // Bind to appropriate framebuffer object
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  // Switch shader program only if necessary
  GLint current_shader = 0;
  glGetIntegerv(GL_CURRENT_PROGRAM, &current_shader); opengl_debug();
  if(shader!=GLuint(current_shader))
    glUseProgram(shader); opengl_debug();

  // Send all uniform values to the shader

  uniform(shader, "rotation", drawable.uniform.transform.rotation);            opengl_debug();
  uniform(shader, "translation", drawable.uniform.transform.translation);      opengl_debug();
  uniform(shader, "color", drawable.uniform.color);                            opengl_debug();
  uniform(shader, "color_alpha", drawable.uniform.color_alpha);                opengl_debug();
  uniform(shader, "scaling", drawable.uniform.transform.scaling);              opengl_debug();
  uniform(shader, "scaling_axis", drawable.uniform.transform.scaling_axis);    opengl_debug();

  return true;
}





void draw(const mesh_drawable& drawable, const camera_scene& camera)
{
    draw(drawable, camera, drawable.shader, drawable.texture_id);
}

void draw(const mesh_drawable& drawable, const camera_scene& camera, GLuint shader)
{
    draw(drawable, camera, shader, drawable.texture_id);
}


void draw(const mesh_drawable& drawable, const camera_scene& user_camera,
          light_animation_data const & light_data, DrawType type) {
  switch(type) {
    case DrawType::STANDARD:
      draw(drawable, user_camera, drawable.shader, drawable.texture_id);
      break;
    case DrawType::PASS0:
      draw_pass0(drawable, light_data);
      break;
    case DrawType::PASS1:
      draw_pass1(drawable, user_camera, light_data);
      break;
  }
}


// Generic draw function provided by vcl : for all objects that should not be taken into account
// by two pass rendering.
void draw(const mesh_drawable& drawable, const camera_scene& user_camera, GLuint shader, GLuint texture_id) {
  // render to user screen
  GLuint fbo = 0;
  if( prepare_draw(drawable, shader, fbo) ) {
    // Bind texture only if id != 0
    if(texture_id!=0) {
      assert(glIsTexture(texture_id));
      uniform(shader, "texture_sampler", 0);
      glActiveTexture(GL_TEXTURE0 + 0);
      glBindTexture(GL_TEXTURE_2D, texture_id);  opengl_debug();
    }

    // setup camera
    uniform(shader,"perspective",user_camera.perspective.matrix());         opengl_debug();
    uniform(shader,"view",user_camera.view_matrix());                       opengl_debug();
    uniform(shader,"camera_position",user_camera.camera_position());        opengl_debug();

    uniform(shader, "ambiant", drawable.uniform.shading.ambiant);      opengl_debug();
    uniform(shader, "diffuse", drawable.uniform.shading.diffuse);      opengl_debug();
    uniform(shader, "specular", drawable.uniform.shading.specular);    opengl_debug();
    uniform(shader, "specular_exponent", drawable.uniform.shading.specular_exponent); opengl_debug();
    vcl::draw(*drawable.data); opengl_debug();
  }
}



// two pass draw to enable caustics : draw all opaque objects in light source space with pass 0,
// and then in window space with pass 1.

void draw_pass0(const mesh_drawable& drawable, light_animation_data const & light_data) {
  // Render to light_view_fbo
  if (prepare_draw(drawable, light_data.shader_pass0, light_data.light_view_fbo)) {

    // Prepare camera (viewing from light source)
    uniform(light_data.shader_pass0, "light_perspective", light_data.orthographic_projection_matrix());         opengl_debug();
    uniform(light_data.shader_pass0, "light_view", light_data.light_camera.view_matrix());                       opengl_debug();

    vcl::draw(*drawable.data); opengl_debug();
  }
}


void draw_pass1(const mesh_drawable& drawable, const camera_scene& user_camera, light_animation_data const & light_data) {
  // Render to screen
  GLuint const fbo = 0;
  if (prepare_draw(drawable, light_data.shader_pass1, fbo)) {
    // preparation is successful : drawing is possible

    // Prepare cameras
    // User camera
    uniform(light_data.shader_pass1, "perspective", user_camera.perspective.matrix());         opengl_debug();
    uniform(light_data.shader_pass1, "view", user_camera.view_matrix());                       opengl_debug();
    uniform(light_data.shader_pass1, "camera_position", user_camera.camera_position());        opengl_debug();

    // Light camera
    uniform(light_data.shader_pass1, "light_perspective", light_data.orthographic_projection_matrix());         opengl_debug();
    uniform(light_data.shader_pass1, "light_view", light_data.light_camera.view_matrix());                       opengl_debug();
    uniform(light_data.shader_pass1, "light_position", light_data.light_camera.camera_position());        opengl_debug();
    uniform(light_data.shader_pass1, "light_view_size", light_data.view_size); opengl_debug();

    // Fog parameters
    uniform(light_data.shader_pass1, "fog_color", light_data.fog_color); opengl_debug();
    uniform(light_data.shader_pass1, "fog_intensity_exp", light_data.fog_intensity_exp); opengl_debug();
    uniform(light_data.shader_pass1, "fog_intensity_linear", light_data.fog_intensity_linear); opengl_debug();


    // Bind object texture only if id != 0
    if(drawable.texture_id != 0) {
      assert(glIsTexture(drawable.texture_id));
      uniform(light_data.shader_pass1, "object_texture_sampler", 0);
      glActiveTexture(GL_TEXTURE0 + 0);
      glBindTexture(GL_TEXTURE_2D, drawable.texture_id);
      opengl_debug();
    }

    // Bind caustics texture
    uniform(light_data.shader_pass1, "caustics_texture_sampler", 1);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, light_data.caustics_sprite_id);  opengl_debug();

    // Bind texture storing depths to light source
    uniform(light_data.shader_pass1, "depths_in_light_ref_sampler", 2);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, light_data.depth_texture_id);  opengl_debug();


    // caustics_depth_texture_sampler[x',y'] = depth of closest fragment (to light source)
    // at position (x',y') when viewing from light source.

    uniform(light_data.shader_pass1, "ambiant", drawable.uniform.shading.ambiant);      opengl_debug();
    uniform(light_data.shader_pass1, "diffuse", drawable.uniform.shading.diffuse);      opengl_debug();
    uniform(light_data.shader_pass1, "specular", drawable.uniform.shading.specular);    opengl_debug();
    uniform(light_data.shader_pass1, "specular_exponent", drawable.uniform.shading.specular_exponent); opengl_debug();
    vcl::draw(*drawable.data); opengl_debug();
  }
}



}
