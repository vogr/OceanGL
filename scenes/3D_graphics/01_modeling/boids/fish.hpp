#pragma once

#include "main/scene_base/base.hpp"
#include "Boid.hpp"
#include <math.h>

#ifdef SCENE_FISH

// Stores some parameters that can be set from the GUI
struct gui_scene_structure
{
    bool wireframe;
};



struct scene_model : scene_base
{

    /** A part must define two functions that are called from the main function:
     * setup_data: called once to setup data before starting the animation loop
     * frame_draw: called at every displayed frame within the animation loop
     *
     * These two functions receive the following parameters
     * - shaders: A set of shaders.
     * - scene: Contains general common object to define the 3D scene. Contains in particular the camera.
     * - data: The part-specific data structure defined previously
     * - gui: The GUI structure allowing to create/display buttons to interact with the scene.
    */

    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);

    void set_gui();
    void set_gui(vcl::timer_event& timer);

    void keyboard_input(scene_structure& scene, GLFWwindow* window, int key, int scancode, int action, int mods);



    std::vector<Boid> all_fish;

    vcl::mesh_drawable surface;
    vcl::mesh_drawable cube;
    vcl::mesh_drawable cone;

    vcl::mesh_drawable fish;
    GLuint texture_fish;

    vcl::mesh_drawable yellow_fish;
    GLuint texture_yellow_fish;

    vcl::timer_event timer;    // Timer allowing to indicate periodic events

    gui_scene_structure gui_scene;
};

#endif


