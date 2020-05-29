
#include "fish.hpp"


#include<iostream>
#include <random>
#include <chrono>
#include <algorithm>





#ifdef SCENE_FISH

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;






/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_model::setup_data(std::map<std::string,GLuint>& , scene_structure& , gui_structure& )
{
    // Create a surface with (u,v)-texture coordinates

    std::mt19937_64 rng;
    // initialize the random number generator with time-dependent seed
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
    // initialize a uniform distribution between 0 and 1
    std::uniform_real_distribution<double> unif(-2, 2);


    for(int i = 0; i < 50; i++){
            double x = unif(rng); //[-2,2]
            double y = unif(rng); //[-2,2]
            double z = unif(rng); //[-2,2]
            vcl::vec3 p = vec3(x,y,z);

            x = unif(rng);
            y = unif(rng);
            z = unif(rng);
            vcl::vec3 d = vec3(x,y,z);

            Boid f(p,d);
            all_fish.push_back(f);

    }

    //fish = mesh_drawable{mesh_load_file_obj("../assets/Fish_v2/fish.obj")};
    //fish.uniform.transform.scaling = 0.2;



    cone = mesh_drawable{mesh_primitive_cone(0.1,{0,0,0}, {0,0,0.1})};
    cone.uniform.color={0,1,0};


    // Load a texture image on GPU and stores its ID
    //texture_fish = create_texture_gpu( image_load_png("../assets/Fish_v2/fish_texture.png"));
}



/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    timer.update();

    // Simulation time step (dt)
    const float dt = timer.update(); // dt: Elapsed time between last frame;

    set_gui(timer);



    // Before displaying a textured surface: bind the associated texture id

    /***********************/

   //glBindTexture(GL_TEXTURE_2D, texture_fish);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);



    for(Boid& f : all_fish){
        f.update(all_fish, dt*10000);
        cone.uniform.transform.rotation = rotation_between_vector_mat3({0,0,1}, f.direction);
        cone.uniform.transform.translation = f.position;
        draw(cone, scene.camera, shaders["mesh"]);
    }

    //glBindTexture(GL_TEXTURE_2D, scene.texture_white);

}


void scene_model::set_gui(timer_event& timer)
{
    // Can set the speed of the animation
    float scale_min = 0.05f;
    float scale_max = 2.0f;
    ImGui::SliderScalar("Time scale", ImGuiDataType_Float, &timer.scale, &scale_min, &scale_max, "%.2f s");

    // Start and stop animation
    if (ImGui::Button("Stop"))
        timer.stop();
    if (ImGui::Button("Start"))
        timer.start();
}



#endif

