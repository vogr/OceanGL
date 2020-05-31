#include "AllBoidsManager.hpp"

AllBoidsManager::AllBoidsManager(unsigned int _n_divisions)
: n_divisions{_n_divisions}
{
 space_grid.resize(n_divisions);
 for (auto & v : space_grid) {
   v.resize(n_divisions);
 }
}

// Cell id :
// We take the coordiantes of the boids in the cube (with values in  [0,1]^2)
// x = boid.position.x
//

vcl::vec2 AllBoidsManager::get_coordinates_in_space_grid(Boid const & boid) const {
  // Coordinates of the boid in the cube
  // The center is at coordinates (0.5,0.5)
  float const cx = (boid.position.x - space_grid_center.x) / space_grid_size + 0.5f;
  float const cy = (boid.position.y - space_grid_center.y) / space_grid_size + 0.5f;
  return {cx, cy};
}


std::tuple<unsigned int,unsigned int> AllBoidsManager::get_cell_id(Boid const & boid) const {
  vcl::vec2 const coords = get_coordinates_in_space_grid(boid);

  unsigned int const i = static_cast<unsigned int>(std::floor(coords.x * n_divisions)) % n_divisions;
  unsigned int const j = static_cast<unsigned int>(std::floor(coords.y * n_divisions)) % n_divisions;
  return {i,j};
}


std::tuple<unsigned int,unsigned int> AllBoidsManager::get_cell_id_in_dual_grid(Boid const & boid) const {
  vcl::vec2 const coords = get_coordinates_in_space_grid(boid);
  // dual grad : shift by half a cell size
  // = 1. / (2 * n_divisions)
  // so that cell (0,0) in main grid is covered by a quarter of
  // cells (0,0), (0,1), (1,0), (1,1) in dual grid

  vcl::vec2 const dual_coords = coords + 1.f / (2 * n_divisions);

  // Dual cell id
  // Warparound :
  // cell (0,0) in dual grid covers cells
  // (0,0), (0, n_div-1), (n_div-1,0), (n_div-1,n_div-1)
  unsigned int const i = static_cast<unsigned int>(std::floor(dual_coords.x * n_divisions)) % n_divisions;
  unsigned int const j = static_cast<unsigned int>(std::floor(dual_coords.y * n_divisions)) % n_divisions;
  return {i,j};
}

std::vector<std::reference_wrapper<Boid>> AllBoidsManager::get_boids_in_cell(unsigned int i, unsigned int j) const {
  return space_grid[i][j];
}


void AllBoidsManager::warparound(Boid & boid) const {
  // Warparound on sides (not top bottom) in cube centered "center" with side lengths (2*warparaound_length)
  float ox = boid.position.x - space_grid_center.x;
  float oy = boid.position.y - space_grid_center.y;

  float L = space_grid_size / 2;

  if (ox > L) {
    boid.position.x = space_grid_center.x - L;
  }
  else if (ox < -L) {
    boid.position.x = space_grid_center.x + L;
  }

  if (oy > L) {
    boid.position.y = space_grid_center.y - L;
  }
  else if (oy < -L) {
    boid.position.y = space_grid_center.y + L;
  }

}

void AllBoidsManager::update_space_grid(vcl::vec3 const & _new_center) {
  // Update center
  space_grid_center = {_new_center.x, _new_center.y};

  // Empty space grid
  for(auto & l : space_grid) {
    for (auto & c : l) {
      c.clear();
    }
  }

  // Warparound boids to valid coordinates
  // And add them in cells of the space grid
  for (auto & boid : all_boids) {
    warparound(boid);
    auto c = get_cell_id(boid);
    space_grid[std::get<0>(c)][std::get<1>(c)].emplace_back(boid);
  }
}



void AllBoidsManager::update_applied_forces(Boid & boid, float dt) const{

  // The space around the players in divided into boids_manager.n_divisions^2 cells.
  // We will only iterate on boids in cells around the current boid.
  // To get these cells, we consider the dual grid
  auto dual_cell_id = get_cell_id_in_dual_grid(boid);

  // Once we have the position of the boid in the dual grid, we only need to consider
  // the four cells in the original grid covered by the cell in the dual grid.

  // Number of neighboring boids affecting the current boid
  int numPerceivedFlockmates = 0;

  // Sum all directions from neighbors to find avg direction (alignment)
  vcl::vec3 accumulated_nearby_flock_directions {0,0,0};

  // Sum all positions from neighbor to steer towards avg position of neighbors (cohesion)
  vcl::vec3 accumulated_nearby_flock_positions {0,0,0};

  // If the current boid A has a neighbor B, it will try to move in the direction
  // of u_BA = vec(BA) / norm(BA) to move away from B. The weight of the vector
  // is the inverse of the distance BA. The separation vector is
  // 1 / norm(BA)^2 * BA.
  // (or for more continuity)
  // (radius - d) / radius * 1/d * BA
  // (separation)
  vcl::vec3 accumulated_weighted_escape_directions {0,0,0};


  for(unsigned int di = 0; di <= 1; di++) {
    for(unsigned int dj = 0; dj <= 1; dj++) {

      // Warparound at grid border
      unsigned int i = (std::get<0>(dual_cell_id) + di) % n_divisions;
      unsigned int j = (std::get<1>(dual_cell_id) + dj) % n_divisions;

      // Iterate over all boids in the cell (i,j)
      for (Boid const & fish : get_boids_in_cell(i,j)) {
        // Do not compare fish with itself !
        if (&fish == &boid) {
          continue;
        }
        vcl::vec3 this_to_fish = fish.position - boid.position;
        float distance = vcl::norm(this_to_fish);

        if(distance < boids_settings.radius_of_vision){
          // Normalised vector from the current boid to its neighbor
          vcl::vec3 fish_dir = this_to_fish / distance;

          // Do not use trigonometric functions (e.g. acos) if we can avoid it.
          // Test if neighboring fish is in field of view
          float cos_angle = vcl::dot(boid.direction, fish_dir);
          if(cos_angle < boids_settings.min_cos_in_vision){
            // Neighboring fish is not in field of view
            continue;
          }
          numPerceivedFlockmates++;
          // weight by inverse of distance
          {
            // float w = 1.f / distance;
            float w = (boids_settings.radius_of_vision - distance) / boids_settings.radius_of_vision;
            w = w * w;
            accumulated_weighted_escape_directions += (boids_settings.radius_of_vision - distance) * w * (-fish_dir);
          }
          accumulated_nearby_flock_directions += fish.direction;
          accumulated_nearby_flock_positions += fish.position;
        }
      }
    }
  }
  
  vcl::vec3 separationDirection, avgFlockDirection, cohesionDirection;
  if(numPerceivedFlockmates > 0){
    separationDirection = vcl::normalize(accumulated_weighted_escape_directions);
    avgFlockDirection = vcl::normalize(accumulated_nearby_flock_directions);
    vcl::vec3 center_nearby_flock = accumulated_nearby_flock_positions / numPerceivedFlockmates;
    cohesionDirection = vcl::normalize(center_nearby_flock - boid.position);
  }
  else {
    separationDirection = boid.direction;
    avgFlockDirection = boid.direction;
    cohesionDirection = boid.direction;
  }

  // Total steering force
  boid.applied_forces = {0,0,0};

  if (numPerceivedFlockmates > 0) {
    auto alignmentForce = boid.steer_towards(avgFlockDirection, dt) * boids_settings.alignmentWeight;
    auto cohesionForce = boid.steer_towards(cohesionDirection, dt) * boids_settings.cohesionWeight;
    auto separationForce = boid.steer_towards(separationDirection, dt) * boids_settings.separationWeight;

    //std::cout << "align:" << norm(alignmentForce) << " c: " << norm(cohesionForce) << " s: " << norm(separationForce);
    boid.applied_forces += alignmentForce;
    boid.applied_forces += cohesionForce;
    boid.applied_forces += separationForce;
  }

  // Stay inside slice of acceptable z values
  if(boid.position.z < boids_settings.z_min) {
    boid.applied_forces += boid.steer_towards({0,0,1}, dt) * boids_settings.stayInBoundsWeight;
  }
  else if(boid.position.z > boids_settings.z_max) {
    boid.applied_forces += boid.steer_towards({0,0,-1}, dt) * boids_settings.stayInBoundsWeight;
  }


  boid.applied_forces += boid.steer_towards(vcl::normalize(vcl::vec3{0,5, 15} - boid.position), dt) * 0.1;

  /*
    if (IsHeadingForCollision()) {
        vcl::vec3 collisionAvoidDir = ObstacleRays();
        vcl::vec3 collisionAvoidForce = steer_towards(collisionAvoidDir, dt) * boids_settings.avoidCollisionWeight;
        std::cout << " av:" << norm(collisionAvoidForce) << "\n";
      F += collisionAvoidForce;
    }
    */

  // Preference for horizontality :
  if (std::abs(boid.direction.z) > 0.2) {
    // Steer towards horizontal place
    boid.applied_forces += boid.steer_towards(vcl::normalize(vcl::vec3{boid.direction.x, boid.direction.y, 0.}), dt) * boids_settings.horizontality_weight;
  }

  // Shark avoidance
  {
    vcl::vec3 avoidance_dir_weighted = boid.avoid_all_obstacles_weighted_dir(shark_refs);
    float w = vcl::norm(avoidance_dir_weighted);

    if (w != 0.f) {
      vcl::vec3 avoidance_dir = avoidance_dir_weighted / w;
      boid.applied_forces += boid.steer_towards(avoidance_dir, dt) * w * boids_settings.avoidCollisionWeight;
    }
  }

  /*
  // Obstacle avoidance
  {
    vcl::vec3 avoidance_dir_weighted = boid.avoid_all_obstacles_weighted_dir(obstacles_to_consider);
    float w = vcl::norm(avoidance_dir_weighted);

    if (w != 0.f) {
      vcl::vec3 avoidance_dir = avoidance_dir_weighted / w;
      boid.applied_forces += boid.steer_towards(avoidance_dir, dt) * w * boids_settings.avoidCollisionWeight;
    }
  }
  */
}



void AllBoidsManager::update_all_boids(vcl::vec3 const &_new_center, float dt) {
  // Move grid and warparound positions of boids outside the boundaries
  update_space_grid(_new_center);
  // Update all forces (the space grid needs no modifications between iterations)
  for (Boid & boid : all_boids) {
    update_applied_forces(boid, dt);
  }
  // Update all positions, velocities and directions : the space grid becomes
  // invalid, but is not necessary for these updates
  for (Boid & boid : all_boids) {
    boid.integrate_forces(dt);
  }
}


void AllBoidsManager::add_boid(vcl::vec3 position, vcl::vec3 direction) {
  all_boids.emplace_back(boids_settings, position, direction);
}

void AllBoidsManager::draw_all_boids(const vcl::camera_scene &camera, const vcl::light_animation_data &light_data, vcl::DrawType draw_type) {
  for (auto & f : all_boids) {
    fish_model.uniform.transform = {
            f.position,
            vcl::mat3{f.dir_right, f.dir_up, f.direction}
    };
    draw(fish_model, camera, light_data, draw_type);
  }
}
