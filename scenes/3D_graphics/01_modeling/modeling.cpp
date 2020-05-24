
#include "modeling.hpp"
#include "helpers/trajectory_creators.h"

#ifdef SCENE_3D_GRAPHICS

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;




/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_model::setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& ) {

  // Setup initial camera mode and position
  scene.camera.camera_type = camera_control_spherical_coordinates;
  scene.camera.scale = 10.0f;
  scene.camera.apply_rotation(0, 0, 0, 1.2f);

  int const N_TREES = 240;
  double const TREE_RADIUS = 0.2;

  // Create visual terrain surface
  terrain = create_terrain(create_texture_gpu( image_load_png("scenes/3D_graphics/01_modeling/assets/floor.png"), GL_REPEAT, GL_REPEAT));
  tree_model = std::make_shared<hierarchy_mesh_drawable>(TreeElement::create_tree_model(scene.texture_white));
  shark_model = mesh_drawable{mesh_load_file_obj("scenes/3D_graphics/01_modeling/assets/shark/Shark.obj")};
  shark_model.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/01_modeling/assets/shark/greatwhiteshark.png"), GL_REPEAT, GL_REPEAT);
  shark = AnimatedFish(shark_model);
  shark.trajectory.init(make_shark_trajectory_keyframes());
  bird.build(shaders, make_shark_trajectory_keyframes());
  
  /*
  for(int i = 0 ; i < N_TREES; i++) {
    vec3 next_pos = get_available_position(elements, TREE_RADIUS);
    auto el = TreeElement{tree_model, affine_transform{next_pos}, TREE_RADIUS};
    elements.push_back(std::move(el));
  }
  */

  // Setup light data
  scene.light_data = light_animation_data{shaders["mesh_depth_pass"], shaders["mesh_draw_pass"]};

  // Load the 32 sprites of the the caustics animation
  std::string const root = "scenes/3D_graphics/01_modeling/assets/caustics/caust";
  std::string const ext = ".png";
  for (int i = 0; i < 32; i++) {
    char id[3];
    sprintf(id, "%02d", i);
    caustics_animation_sprites_ids[i] = create_texture_gpu(
            image_load_png(root + id + ext),
            GL_REPEAT, GL_REPEAT
    );
  }
  // Setup timer to cycle through all the 32 sprites
  caustics_animation_timer.scale = 20.;
  caustics_animation_timer.t_min = 0.;
  caustics_animation_timer.t_max = 32.;
  caustics_animation_timer.start();
}



/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui)
{

  /**
   * Animation loop
   */
  shark.update();
  // choose correct sprite in light animation
  {
    caustics_animation_timer.update();
    int caustics_sprite_number = static_cast<int>(caustics_animation_timer.t);
    scene.light_data.caustics_sprite_id = caustics_animation_sprites_ids[caustics_sprite_number];
  }



  /**
   * Rendering loop
   */
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  set_gui();
  glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe
  glPolygonOffset( 1.0, 1.0 );

  TreeElement tree {tree_model, affine_transform{evaluate_terrain(0.2,0.3)}, 1.f};


  /**
   * Two pass rendering :
   * - pass 0 : "draw" depth information from the point of view
   *    of the light source (scene.light_data.light_camera) to an off-screen
   *    framebuffer. The depth information is "drawn" to a texture with ID
   *    scene.light_data.depth_texture_id.
   * - pass 1 : draw the scene from the point of view of the user camera
   *    to the framebuffer of the screen. Use the data from the off-screen
   *    framebuffer to draw shadows.
   */

  // Begin with a cleared off-screen framebuffer to store depth information
  glBindFramebuffer(GL_FRAMEBUFFER, scene.light_data.light_view_fbo);
  glClear(GL_DEPTH_BUFFER_BIT);
  int width = scene.light_data.light_camera.perspective.get_width(), height = scene.light_data.light_camera.perspective.get_height();

  for (auto pass : {DrawType::PASS0, DrawType::PASS1}) {
    if(pass == DrawType::PASS1) {
      // pass 1 : draw to screen
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glfwGetWindowSize(gui.window, &width, &height);
    }
    // The viewport is different for the screen FBO and for the depth FBO,
    // set correct viewport before drawing.
    glViewport(0, 0, width, height);

    draw(terrain, scene.camera, scene.light_data, pass);
    for(vec2 uv : {vec2{0.2, 0.3}, {0.2,0.9}, {0.8, 0.2}, {0.4,0.6}, {0.7, 0.5}}) {
      tree.transform = affine_transform{evaluate_terrain(uv.x, uv.y)};
      tree.draw(scene.camera, scene.light_data, pass);
      shark.draw(scene.camera, scene.light_data, pass);
      bird.draw(scene.camera, scene.light_data, pass);
    }

    //shark.trajectory.draw(scene.camera, shaders);

  }



}




void scene_model::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);
}



#endif

