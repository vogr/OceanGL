
#include "controls.hpp"

#include "../modeling.hpp"
#include "../terrain/terrain.h"


void CameraPhysics::move_in_dir(vcl::camera_scene & camera, vcl::vec3 move_dir, float dt) {
  /* Give impulsion to camera in the direction given by `move_dir` */

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
  /* Move camera in direction given by `move_dir` and handle collisions with ground */

  move_in_dir(camera, move_dir, dt);

  // Find terrain height where player stands
  vcl::vec2 uv = terrain_xy_to_uv({-camera.translation.x, -camera.translation.y});
  float terrain_height = evaluate_terrain_z(uv.x, uv.y);

  // Stop before distance to ground is 0, leave a gap
  float my_height = 0.1;

  if ((-camera.translation.z) < (terrain_height + my_height)) {
    /* Collision ! Move back up, and remove normal component from velocity */

    camera.translation.z = - (terrain_height + my_height);
    vcl::vec3 tn = terrain_normal(uv.x,uv.y);

    // v in view coords -> O*v in world coords
    // v = O.t ( O*v - dot((O*v), N)N) where N is terrain normal in world coordinates
    // v = v - dot((O*v), N) O.t*N

    // here we slide with minimal energy loss
    velocity = 0.95 * (velocity - vcl::dot(camera.orientation * velocity, tn) * vcl::transpose(camera.orientation) * tn);
  }
}

vcl::vec3 get_move_dir_from_user_input(GLFWwindow* window) {
  // Prepare a movement direction vector by adding a vector for each key that is currently being held
  // by the player, then normalize the vector obtained (and multiply by a factor if sprint is held)
  // ZQSD + SPACE + LCTRL for movement
  // Shift for sprint

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

// keyboard_input(scene_structure& scene, GLFWwindow* window, int key, int scancode, int action, int )
void scene_model::keyboard_input(scene_structure& , GLFWwindow* window, int key, int , int action, int) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    /* Escape key -> quit game */
    std::cerr << "Stopping now.\n";
    abort();
  }
  else if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
    /* F11 -> Switch between fullscreen and windowed */
    if (glfwGetWindowMonitor(window) == nullptr) {
      // Windowed mode, switch to fullscreen
      auto primary_monitor = glfwGetPrimaryMonitor();
      auto video_mode = glfwGetVideoMode(primary_monitor);
      glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, video_mode->width, video_mode->height, GLFW_DONT_CARE);
      // is resize callback called ?
      //scene.camera.perspective.set_dims(width_pm, height_pm);
    }
    else {
      // currently fullscreen, set windowed
      glfwSetWindowMonitor(window, nullptr, 0, 0, 1280, 720 , GLFW_DONT_CARE);
    }
  }
  else if (key == GLFW_KEY_F6 && action == GLFW_PRESS) {
    /* F6 -> Switch between captive pointer and free pointer */
    auto cursor = glfwGetInputMode(window, GLFW_CURSOR);
    if (cursor == GLFW_CURSOR_DISABLED) {
      // Re-enable cursor
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else {
      // Hide cursor
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      // Use raw cursor input
      //if (glfwRawMouseMotionSupported()) {
      //  glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
      //}
      // Disable mouse
    }

  }
}
