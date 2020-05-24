
#include "controls.hpp"

#include "../modeling.hpp"
#include "../terrain/terrain.h"


void CameraPhysics::move_in_dir(vcl::camera_scene & camera, vcl::vec3 move_dir, float dt) {

  vcl::vec3 impulsion = 15 * move_dir;
  vcl::vec3 friction = - 0.8 * velocity;
  vcl::vec3 F = impulsion + friction;

  // Symplectic Euler
  velocity += dt * F;

  vcl::vec3 tr = dt * velocity;

  camera.apply_translation_in_screen_plane(tr.x, tr.y);
  camera.apply_translation_orthogonal_to_screen_plane(- tr.z);

}

void CameraPhysics::move_and_slide(vcl::camera_scene & camera, vcl::vec3 move_dir, float dt) {
  move_in_dir(camera, move_dir, dt);
  vcl::vec2 uv = terrain_xy_to_uv({-camera.translation.x, -camera.translation.y});

  float terrain_height = evaluate_terrain_z(uv.x, uv.y);
  float my_height = 0.1;
  std::cout << "vbefore=" << camera.orientation * velocity << "\n\n";

  if ((-camera.translation.z) < (terrain_height + my_height)) {
    camera.translation.z = - (terrain_height + my_height);
    vcl::vec3 tn = terrain_normal(uv.x,uv.y);
    float vnorm = vcl::norm(velocity);
    // v in view coords -> O*v in world coords
    // v = O.t ( O*v - dot((O*v), N)N) where N is terrain normal in world coordinates
    // v = v - dot((O*v), N) O.t*N

    // here we slide with minimal energy loss
    velocity = 0.95 * (velocity - vcl::dot(camera.orientation * velocity, tn) * vcl::transpose(camera.orientation) * tn);
  }
}

vcl::vec3 get_move_dir_from_user_input(GLFWwindow* window) {
  vcl::vec3 move_dir = {0,0,0};
  if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
    // Forward
    move_dir += {0,0,-1};
  }
  if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
    // Backward
    move_dir += {0,0,1};
  }
  if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
    // Right
    move_dir += {-1, 0, 0};
  }
  if (glfwGetKey(window, GLFW_KEY_A ) == GLFW_PRESS){
    // Left
    move_dir += {1, 0, 0};
  }
  else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    // Up
    move_dir += {0, -1 , 0};
  }
  else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
    move_dir += {0, 1 , 0};
  }

  float sprint = 1.f;
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    sprint = 5.f;
  }

  float move_dir_norm = vcl::norm(move_dir);
  if (move_dir_norm > 0) {
    return sprint * move_dir / move_dir_norm;
  }
  else {
    return {0,0,0};
  }
}


void scene_model::keyboard_input(scene_structure& scene, GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    std::cerr << "Stopping now.\n";
    abort();
  }
}
