

#include "billboards.h"

using namespace vcl;


FlatBillboardMesh::FlatBillboardMesh(GLuint texture_id) {
  struct mesh billboard_cpu;
  billboard_cpu.position     = {{-0.4f,0,0}, { 0.4f,0,0}, { 0.4f, 0.8f,0}, {-0.4f, 0.8f,0}};
  billboard_cpu.texture_uv   = {{0,1}, {1,1}, {1,0}, {0,0}};
  billboard_cpu.connectivity = {{0,1,2}, {0,2,3}};


  billboard = mesh_drawable{billboard_cpu};
  billboard.uniform.shading = {1,0,0}; // set pure ambiant component (no diffuse, no specular) - allow to only see the color of the texture
  // Load a texture (with transparent background)
  billboard.texture_id = texture_id;

}

CrossBillboardMesh::CrossBillboardMesh(GLuint texture_id) {
  struct mesh billboard_cpu;
  billboard_cpu.position     = {{-0.2f,0,0}, { 0.2f,0,0}, { 0.2f, 0.f,0.4f}, {-0.2f, 0.0f,0.4f}};
  billboard_cpu.texture_uv   = {{0,1}, {1,1}, {1,0}, {0,0}};
  billboard_cpu.connectivity = {
          {0,1,2}, {0,2,3},   // front
          {2,1,0}, {3,2,0}    // back
  };

  billboard = mesh_drawable{billboard_cpu};
  billboard.uniform.shading = {1,0,0}; // set pure ambiant component (no diffuse, no specular) - allow to only see the color of the texture
  // Load a texture (with transparent background)
  billboard.texture_id = texture_id;

}


void FlatBillboard::draw(const camera_scene& camera, GLuint shader) {
  // Enable use of alpha component as color blending for transparent elements
  //  new color = previous color + (1-alpha) current color
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Disable depth buffer writing
  //  - Transparent elements cannot use depth buffer
  //  - They are supposed to be display from furthest to nearest elements
  glDepthMask(false);
  // Display a billboard always facing the camera direction
  // ********************************************************** //
  model.billboard.uniform.transform.rotation = camera.orientation;
  model.billboard.uniform.transform.translation = transform.translation;
  vcl::draw(model.billboard, camera, shader);

  glDepthMask(true);
}

void CrossBillboard::draw(const camera_scene& camera, GLuint shader) {
  // Enable use of alpha component as color blending for transparent elements
  //  new color = previous color + (1-alpha) current color

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Disable depth buffer writing
  //  - Transparent elements cannot use depth buffer
  //  - They are supposed to be display from furest to nearest elements
  glDepthMask(false);
  // Display two orthogonal billboards with static orientation
  // ********************************************************** //
  mat3 const identity = mat3::identity();
  mat3 const  R = rotation_from_axis_angle_mat3({0,0,1}, static_cast<float>(M_PI / 2.0)); // orthogonal rotation

  model.billboard.uniform.transform = transform;

  model.billboard.uniform.transform.rotation = identity;
  vcl::draw(model.billboard, camera, shader);


  model.billboard.uniform.transform.rotation = R * model.billboard.uniform.transform.rotation;
  vcl::draw(model.billboard, camera, shader);

  glDepthMask(true);
}
