
#include "modeling.hpp"
#include "helpers/trajectory_creators.h"


#ifdef SCENE_3D_GRAPHICS

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;




/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_model::setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure&) {
  /** Setup user camera */
  scene.camera.scale = 0.0f;
  // Initial position :
  scene.camera.translation = {0,0,-10};
  scene.camera.apply_rotation(0, 0, 0, M_PI / 3);

  /** Setup light source */
  scene.light_data = light_animation_data{shaders["mesh_depth_pass"], shaders["mesh_draw_pass"]};
  scene.light_data.fog_intensity_exp = 0.013;
  scene.light_data.fog_intensity_linear = 0.10;

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

  // Prepare shark model
  shark_model = mesh_drawable{mesh_load_file_obj("scenes/3D_graphics/01_modeling/assets/shark/Shark.obj")};
  shark_model.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/01_modeling/assets/shark/greatwhiteshark.png"), GL_REPEAT, GL_REPEAT);
  shark_model.uniform.shading.specular = 0.1;
  shark_model.normal_texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/01_modeling/assets/shark/greatwhiteshark_n.png"), GL_REPEAT, GL_REPEAT);

  // Prepare fish model
  fish_model = mesh_drawable{mesh_load_file_obj("scenes/3D_graphics/01_modeling/assets/Fish_v2/fish.obj")};
  fish_model.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/01_modeling/assets/Fish_v2/fish_texture.png"), GL_REPEAT, GL_REPEAT);

  /** Animated models */


  // Boids
  int const N_DIVISIONS = 120;
  boids_manager = AllBoidsManager(N_DIVISIONS);
  // Ideally we need : radius_of_vision < (space_grid_size / N_DIVISIONS)
  boids_manager.space_grid_size = 600.f;
  boids_manager.boids_settings.radius_of_vision = 5.f;

  boids_manager.boids_settings.maxSpeed = 24;
  boids_manager.boids_settings.maxSteerForce = 7;
  boids_manager.boids_settings.alignmentWeight = 1.263;
  boids_manager.boids_settings.cohesionWeight = 1.521;
  boids_manager.boids_settings.separationWeight = 1.675;


  boids_manager.fish_model = fish_model;
  size_t const N_BOIDS = 1200;
  for(size_t i = 0; i < N_BOIDS; i++){
    float L = 10.f;
    vcl::vec3 p {rand_interval(-L,L),rand_interval(-L,L),30 + rand_interval(-L,L)};
    vcl::vec3 d {rand_interval(-1,1),rand_interval(-1,1),rand_interval(-0.5,0.5)};
    boids_manager.add_boid(p,d);
  }

  // Sharks
  shark_manager = SharksManager(shark_model);

  /** Prepare chunk loader */

  int const RENDER_RADIUS = 6;
  terrain = ChunkLoader{
          create_texture_gpu( image_load_png("scenes/3D_graphics/01_modeling/assets/floor.png"), GL_REPEAT, GL_REPEAT),
          RENDER_RADIUS
  };
  terrain.n_billboards_per_chunk = 6;
  terrain.shark_spawn_probability = 0.12;



  // Restart main_timer to prevent huge dt on first draw
  main_timer.update();
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
   // Load the chunks around the player, eventually spawn sharks
   auto p = scene.camera.camera_position();
   terrain.update_center(p, shark_manager);
   // Move light source (because caustics texture are only visible in a small radius around the light source)
   scene.light_data.light_camera.translation = {-p.x, -p.y, -150};
 }
 {
   // choose correct sprite in caustics animation
   caustics_animation_timer.update();
   int caustics_sprite_number = static_cast<int>(caustics_animation_timer.t);
   scene.light_data.caustics_sprite_id = caustics_animation_sprites_ids[caustics_sprite_number];
 }


  // Update shark and fish positions
  shark_manager.update_all_sharks();

  // Make boids consider sharks
  boids_manager.shark_refs = shark_manager.get_shark_refs();

  // Move boids (+ warparounf in cube centered on camera position)
  boids_manager.update_all_boids(scene.camera.camera_position(), dt);
  

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

    shark_manager.draw_all_sharks(scene.camera, scene.light_data, pass);

    boids_manager.draw_all_boids(scene.camera, scene.light_data, pass);

    terrain.draw(scene.camera, scene.light_data, pass);

    }
}




void scene_model::set_gui() {
  ImGui::Checkbox("Wireframe", &gui_scene.wireframe);
  ImGui::SliderFloat("MaxSpeed", &boids_manager.boids_settings.maxSpeed, 0.f, 150.f);
  ImGui::SliderFloat("MaxSteer", &boids_manager.boids_settings.maxSteerForce, 0.f, 150.f);
  ImGui::SliderFloat("AlignmentWeigh", &boids_manager.boids_settings.alignmentWeight, 0.f, 5.f);
  ImGui::SliderFloat("CohesionWeight", &boids_manager.boids_settings.cohesionWeight, 0.f, 5.f);
  ImGui::SliderFloat("SeparationWeight", &boids_manager.boids_settings.separationWeight, 0.f, 5.f);
  ImGui::SliderFloat("Warparound", &boids_manager.space_grid_size, 0.f, 1000.f);
  ImGui::SliderFloat("z_min", &boids_manager.boids_settings.z_min, 0.f, 150.f);
  ImGui::SliderFloat("z_max", &boids_manager.boids_settings.z_max, 0.f, 150.f);
  ImGui::SliderFloat("Radius of visison", &boids_manager.boids_settings.radius_of_vision, 0.f, 100.f);
  ImGui::SliderFloat("Obstacle avoidance dist", &boids_manager.boids_settings.raymarcher.max_depth, 0.f, 100.f);
  ImGui::SliderFloat("Avoidance weight", &boids_manager.boids_settings.avoidCollisionWeight, 0.f, 30.f);
}


#endif

