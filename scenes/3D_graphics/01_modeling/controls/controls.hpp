#pragma once

#include "vcl/vcl.hpp"

class CameraPhysics {
    // velocity in view coordinates ! (-z is forward direction, x is left, y is down)
    vcl::vec3 velocity;
public:
    void move_in_dir(vcl::camera_scene & camera, vcl::vec3 move_dir, float dt);
    void move_and_slide(vcl::camera_scene & camera, vcl::vec3 move_dir, float dt);
};


vcl::vec3 get_move_dir_from_user_input(GLFWwindow* window);
