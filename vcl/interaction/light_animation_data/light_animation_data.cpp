#include <vcl/opengl/debug/opengl_debug.hpp>
#include <vcl/math/math.hpp>
#include "light_animation_data.hpp"


using namespace vcl;

// Setup OpenGl objects for caustics animation.
// Inspiration : shadow mapping tutorial found here https://github.com/opengl-tutorials/ogl/blob/master/tutorial16_shadowmaps/tutorial16.cpp
light_animation_data::light_animation_data(GLuint shader0, GLuint shader1)
: shader_pass0{shader0},
  shader_pass1{shader1}
{

  // Create FBO for depth data when viewed from the light source POV
  opengl_debug();
  glGenFramebuffers(1, &light_view_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, light_view_fbo);
  glObjectLabel(GL_FRAMEBUFFER, light_view_fbo, -1, "Caustics pass fbo");
  opengl_debug();

  // Add a texture to store depth information in the FBO
  glGenTextures(1, &depth_texture_id);
  opengl_debug();
  glBindTexture(GL_TEXTURE_2D, depth_texture_id);
  opengl_debug();

  glObjectLabel(GL_TEXTURE, depth_texture_id, -1, "Caustics depth texture");
  opengl_debug();

  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_FBO_HEIGHT, SHADOW_FBO_WIDTH, 0, GL_DEPTH_COMPONENT,
               GL_FLOAT, nullptr);
  opengl_debug();


  // When `GL_TEXTURE_COMPARE_MODE` is `GL_COMPARE_REF_TO_TEXTURE`
  // then the sampler should be a shadow sampler (in the shaders) !
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);  opengl_debug();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);  opengl_debug();

  // Enable PCF (filtering, to obtain smoother shadow edges)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  opengl_debug();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  opengl_debug();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);  opengl_debug();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);  opengl_debug();


  // add texture to fbo to store depth information
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture_id, 0);  opengl_debug();

  // No color output in the bound framebuffer, only depth.
  glDrawBuffer(GL_NONE);  opengl_debug();
  glReadBuffer(GL_NONE);  opengl_debug();
  glEnable(GL_DEPTH_TEST);  opengl_debug();


  // Check if fbo is correctly constructed
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    abort();
  }

// Setup light camera

  light_camera.orientation = rotation_from_axis_angle_mat3({1, 0, 0}, 20 * M_PI / 180);
  light_camera.translation = {0, 100, -200};
  light_camera.perspective = perspective_structure(3 * M_PI / 2, 1, 0.01f, 500.0f);

}