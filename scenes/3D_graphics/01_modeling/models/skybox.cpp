//
// Created by vogier on 02/04/2020.
//

#include "skybox.h"

using namespace vcl;

struct skybox_mesh {
    mesh back, left, front, right, top, bottom;
};


skybox_models build_skybox() {
  std::array<mesh, 6> skybox_m;

  std::array<vec3, 8> v {{
    {-.5f,-.5f,-.5f},{-.5,.5f,-.5},{.5f,.5f,-.5},{.5f,-.5,-.5},
    {-.5,-.5,.5f},{-.5,.5f,.5f},{.5f,.5f,.5f},{.5f,-.5,.5f}
  }};

  skybox_m[0].position     = {v[7], v[6], v[2], v[3]}; // back
  skybox_m[1].position     = {v[0], v[4], v[7], v[3]}; // left
  skybox_m[2].position     = {v[0], v[1], v[5], v[4]}; // front
  skybox_m[3].position     = {v[5], v[1], v[2], v[6]}; // right
  skybox_m[4].position     = {v[4], v[5], v[6], v[7]}; // top
  skybox_m[5].position     = {v[1], v[0], v[3], v[2]}; // bottom

  for (int i = 0; i < 6 ; i++) {
    // triangles facing INWARDS because the skybox is always around the camera
    skybox_m[i].connectivity = {
            {0,1,2}, {2,3,0}
    };
    skybox_m[i].texture_uv   = {{0,1}, {1,1}, {1,0}, {0,0}};
  }

  std::array<std::string, 6> fnames {"back.png", "left.png", "front.png", "right.png", "top.png", "bottom.png"};
  std::string root  {"scenes/3D_graphics/01_modeling/assets/skybox/"};

  skybox_models skybox;
  for (int i = 0; i < 6; i++) {
    skybox[i] = mesh_drawable{skybox_m[i]};
    skybox[i].uniform.shading = {1,0,0};
    skybox[i].uniform.transform.scaling = 500;

    auto path = root + fnames[i];
    skybox[i].texture_id = create_texture_gpu(image_load_png(path), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  }
  return skybox;
}


void Skybox::draw(const vcl::camera_scene &camera, GLuint shader) {
  for (int i = 0; i < 6; i++) {
    models[i].uniform.transform.translation = camera.camera_position();
    vcl::draw(models[i], camera, shader);
  }
}