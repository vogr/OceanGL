#pragma once

#include "main/scene_base/base.hpp"

#include "models/AnimatedBird.h"
#include "models/AnimatedFish.h"
#include "models/models.h"
#include "models/billboards.h"
#include "models/skybox.h"
#include "terrain/terrain.h"

#include "trajectories/Trajectory.h"

#ifdef SCENE_3D_GRAPHICS

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
    gui_scene_structure gui_scene;

    /**
     * Models and visual elements
     **/
    vcl::mesh_drawable terrain;
    //std::vector<WorldElement> elements;

    std::shared_ptr<hierarchy_mesh_drawable> tree_model;
    mesh_drawable shark_model;

    AnimatedFish shark;
    AnimatedBird bird;
    /************************************
     * OpenGL objects to render caustics
     ************************************/
    // Caustics animation.
    std::array<GLuint,32> caustics_animation_sprites_ids;
    // Animation timer
    timer_interval caustics_animation_timer;

};

#endif


