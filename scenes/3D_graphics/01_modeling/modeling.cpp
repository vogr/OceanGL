
#include "modeling.hpp"
#include "helpers/trajectory_creators.h"


#ifdef SCENE_3D_GRAPHICS

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;




/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_model::setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui) {

  /** Setup user cursor */
  // Press F6 to hide the cursor
  /*
  glfwSetInputMode(gui.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  // Use raw cursor input
  if (glfwRawMouseMotionSupported()) {
    glfwSetInputMode(gui.window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
  }
  */

  /** Setup user camera */
  scene.camera.scale = 0.0f;
  // Initial position :
  scene.camera.translation = {0,0,-10};
  scene.camera.apply_rotation(0, 0, 0, M_PI / 3);

  /** Setup light source */
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


  /** Prepare drawable elements : meshes and textures */

  // Prepare chunk loader
  int const RENDER_RADIUS = 6;
  terrain = ChunkLoader{
    create_texture_gpu( image_load_png("scenes/3D_graphics/01_modeling/assets/floor.png"), GL_REPEAT, GL_REPEAT),
    RENDER_RADIUS
  };
  terrain.n_billboards_per_chunk = 6;

  // Prepare shark
  shark_model = mesh_drawable{mesh_load_file_obj("scenes/3D_graphics/01_modeling/assets/shark/Shark.obj")};
  shark_model.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/01_modeling/assets/shark/greatwhiteshark.png"), GL_REPEAT, GL_REPEAT);
  shark = AnimatedFish(shark_model);
  shark.trajectory.init(make_shark_trajectory_keyframes());
  shark.model.uniform.shading.specular = 0.1;



}



/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data drawing */
void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui)
{

  /**
   * Animation loop
   */
  float dt = main_timer.update();

  //TODO: add back move_and_slide to enable collisions with ground
  //camera_physics.move_and_slide(scene.camera, get_move_dir_from_user_input(gui.window), dt);
  camera_physics.move_in_dir(scene.camera, get_move_dir_from_user_input(gui.window), dt);
  {
    // Load the chunks around the player
    auto p = scene.camera.camera_position();
    terrain.update_center(p);
    // Move light source (because caustics texture are only visible in a small radius around the light source)
    scene.light_data.light_camera.translation = {-p.x, -p.y, -150};
  }

  // Update shark position
  shark.update();

  {
    // choose correct sprite in caustics animation
    caustics_animation_timer.update();
    int caustics_sprite_number = static_cast<int>(caustics_animation_timer.t);
    scene.light_data.caustics_sprite_id = caustics_animation_sprites_ids[caustics_sprite_number];
  }


  /**
   * Rendering loop
   */
   // Cull back faces
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  // Update GUI options
  set_gui();

  //
  glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe
  glPolygonOffset( 1.0, 1.0 );

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

    shark.draw(scene.camera, scene.light_data, pass);
    //bird.draw(scene.camera, scene.light_data, pass);
    terrain.draw(scene.camera, scene.light_data, pass);

  }



}




void scene_model::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);
}



#endif

