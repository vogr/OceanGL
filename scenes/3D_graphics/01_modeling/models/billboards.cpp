

#include "billboards.h"

using namespace vcl;


FlatBillboardMesh::FlatBillboardMesh(GLuint texture_id) {
  struct mesh billboard_cpu;
  billboard_cpu.position     = {{-0.4f,0,0}, { 0.4f,0,0}, { 0.4f, 0.8f,0}, {-0.4f, 0.8f,0}};
  billboard_cpu.texture_uv   = {{0,1}, {1,1}, {1,0}, {0,0}};
  billboard_cpu.connectivity = {{0,1,2}, {0,2,3}};


  billboard = mesh_drawable{std::move(billboard_cpu)};
  billboard.uniform.shading = {1,0,0}; // set pure ambiant component (no diffuse, no specular) - allow to only see the color of the texture
  // Load a texture (with transparent background)
  billboard.texture_id = texture_id;

}

CrossBillboardMesh::CrossBillboardMesh(GLuint texture_id) {
  std::vector<vec2>  const t_xy {{0.4, 0}, {0,0.4}, {-0.4,0}, {0,-0.4}};
  std::vector<float> const t_u  {1, 1, 0, 0};
  for (int i = 0 ; i < 4 ; i ++) {
    auto xy = t_xy[i];
    auto u = t_u[i];
    struct mesh billboard_cpu;
    billboard_cpu.position     = {{0,0,0}, { 0,0,0.8}, { xy.x, xy.y, 0.8f}, {xy.x, xy.y ,0}};
    billboard_cpu.texture_uv   = {{0.5,1}, {0.5,0}, {u,0}, {u,1}};
    billboard_cpu.connectivity = {{0,1,2}, {0,2,3},
                                  {0,2,1}, {0,3,2}};

    billboard[i] = mesh_drawable{std::move(billboard_cpu)};
    billboard[i].uniform.shading = {1,0,0}; // set pure ambiant component (no diffuse, no specular) - allow to only see the color of the texture
    billboard[i].texture_id = texture_id;

  }
}

void CrossBillboardMesh::set_texture_id(GLuint tex_id) {
  for (int i = 0; i < 4; i ++) {
    billboard[i].texture_id = tex_id;
  }
}


void FlatBillboard::draw(const vcl::camera_scene& camera, const light_animation_data & light_data, DrawType draw_type) {
  if (draw_type == DrawType::PASS0) {
    // Billboards do not cast shadows, skip detph testing
    return;
  }

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
  model.billboard.uniform.transform = transform;
  model.billboard.uniform.transform.rotation = camera.orientation;
  vcl::draw(model.billboard, camera, light_data, draw_type);

  glDepthMask(true);
}

void CrossBillboard::draw(const vcl::camera_scene& camera, const light_animation_data & light_data, DrawType draw_type) {

  if (draw_type == DrawType::PASS0) {
    // Billboards do not cast shadows, skip detph testing
    return;
  }

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


  vec3 camera_to_billboard = transform.translation - camera.camera_position();

  float const sx = dot(camera_to_billboard, transform.rotation * vec3{1,0,0});
  float const sy = dot(camera_to_billboard, transform.rotation * vec3{0,1,0});

  for (int i = 0; i < 4; i++) {
    model.billboard[i].uniform.transform = transform;
  }

  if (sy >= 0) {
    if (sx >= 0) {
      vcl::draw(model.billboard[0], camera, light_data, draw_type);
      vcl::draw(model.billboard[1], camera, light_data, draw_type);
      vcl::draw(model.billboard[2], camera, light_data, draw_type);
      vcl::draw(model.billboard[3], camera, light_data, draw_type);
    }
    else {
      vcl::draw(model.billboard[1], camera, light_data, draw_type);
      vcl::draw(model.billboard[2], camera, light_data, draw_type);
      vcl::draw(model.billboard[3], camera, light_data, draw_type);
      vcl::draw(model.billboard[0], camera, light_data, draw_type);
    }
  }
  else {
    if (sx < 0) {
      vcl::draw(model.billboard[2], camera, light_data, draw_type);
      vcl::draw(model.billboard[3], camera, light_data, draw_type);
      vcl::draw(model.billboard[0], camera, light_data, draw_type);
      vcl::draw(model.billboard[1], camera, light_data, draw_type);
    }
    else {
      vcl::draw(model.billboard[3], camera, light_data, draw_type);
      vcl::draw(model.billboard[0], camera, light_data, draw_type);
      vcl::draw(model.billboard[1], camera, light_data, draw_type);
      vcl::draw(model.billboard[2], camera, light_data, draw_type);
    }
  }

  glDepthMask(true);
}
