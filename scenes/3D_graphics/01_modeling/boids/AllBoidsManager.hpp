#pragma once

#include "vcl/vcl.hpp"

#include "Boid.hpp"

class AllBoidsManager {
  using space_grid_type = std::vector<std::vector<std::vector<std::reference_wrapper<Boid>>>>;

  // Store all boids in boid manager
  std::vector<Boid> all_boids;

  // Ideally :
  // space_grid_size / n_divisions >= boid.radius_of_vision
  unsigned int n_divisions = 0;
  vcl::vec2 space_grid_center;

  // Grid where reference to boids will be stored
  space_grid_type space_grid;


  // Move boid to valid coordinates using warparound at grid edges.
  void warparound(Boid & boid) const;
  // Update boid velocity (space grid stays valid)
  void update_applied_forces(Boid & boid, float dt) const;

  // Move space_grid to position _new_center (noramally : the player's position)
  void update_space_grid(vcl::vec3 const & _new_center);
public:
  AllBoidsManager() = default;

  float space_grid_size = 500.;

  vcl::mesh_drawable fish_model;
  BoidSettings boids_settings;


  std::vector<std::reference_wrapper<WorldElement>> shark_refs;
  //std::reference_wrapper<ChunkLoader>

  // Prepare boid manager :
  // - empty space grid
  // - empty boids list
  explicit AllBoidsManager(unsigned int divisions);

  vcl::vec2 get_coordinates_in_space_grid(Boid const & boid) const;
  std::tuple<unsigned int,unsigned int> get_cell_id(Boid const & boid) const;
  std::tuple<unsigned int,unsigned int> get_cell_id_in_dual_grid(Boid const & boid) const;
  std::vector<std::reference_wrapper<Boid>> get_boids_in_cell(unsigned int i, unsigned int j) const;

  void update_all_boids(vcl::vec3 const &_new_center, float dt);

  unsigned int get_n_divisions() const {return n_divisions; };
  void add_boid(vcl::vec3 position, vcl::vec3 direction = {1,0,0});

  void draw_all_boids(const vcl::camera_scene& camera, const vcl::light_animation_data & ca_data, vcl::DrawType draw_type);
};


