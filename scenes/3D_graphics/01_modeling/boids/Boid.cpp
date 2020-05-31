#include "Boid.hpp"

#include <iostream>


#include "../terrain/terrain.h"

/** This part is for defining the boids' behaviour depending on the list of nearby boids*/
Boid::Boid(BoidSettings const & _settings)
: WorldElement{1.},
  settings{_settings},
  position{0.0,0.0,0.0},
  direction{1.0,0.0,0.0}
{
  dir_right = normalize(cross(direction, {0,0,1}));
  dir_up = cross(dir_right, direction);
}

Boid::Boid(BoidSettings const & _settings, vcl::vec3 position_, vcl::vec3 direction_)
: WorldElement{1.f},
  settings{_settings},
  position{position_},
  direction{direction_}
{
  velocity = settings.get().maxSpeed * direction;
  dir_right = normalize(cross(direction, {0,0,1}));
  dir_up = cross(dir_right, direction);
}

vcl::vec3 Boid::steer_towards(vcl::vec3 target_dir, float dt){
    // target dir should be normalized !
    vcl::vec3 desired_velocity = target_dir * settings.get().maxSpeed;

    // F : force that would take boid exactly to desired_velocity after dt
    vcl::vec3 F =  (desired_velocity - velocity) / dt;

    // Scale back F to maxSteerForce if it is too high
    float const n = norm(F);
    if( n > settings.get().maxSteerForce) {
      return settings.get().maxSteerForce * (F / n);
    }
    else {
      return F;
    }
}

vcl::vec3 Boid::obstacle_avoidance_dir(WorldElement const & obstacle) {
  vcl::vec3 boid_to_obstacle = obstacle.getPosition() - position;

  // project obstacle center on boid direction :
  // vector from center to its projection gives escape direction
  float projection_value = vcl::dot(direction, boid_to_obstacle);

  // even if projection_value < 0 (obstacle is behind boid), try to avoid :
  // = try to escape predators


  vcl::vec3 escape_direction;
  if (projection_value < 0) {
    escape_direction = - boid_to_obstacle;
  }
  else {
    escape_direction =(projection_value * direction) - boid_to_obstacle;
  }

  if(norm(escape_direction) == 0.f) {
    // Should be rare ! Boid is facing exactly towards the center of the obstacle. Any unit vector
    // in the plane perpendicular to the direction is an escape direction.
    escape_direction = {-direction.z, -direction.z, direction.x + direction.y};
    // except if d.x = -d.y ... should be exceedingly rare
  }
  return vcl::normalize(escape_direction);
}



vcl::vec3 Boid::avoid_all_obstacles_weighted_dir(std::vector<std::reference_wrapper<WorldElement>> const & obstacles_to_consider) {
  vcl::vec3 accumulated_weighted_avoidance_dirs;

  for (int T = -1 ; T <= 1 ; T++) {
    for (int N = -1; N <= 1; N++) {
      for(int B = -1 ; B <= 1; B++) {

        // Don't raymarch zero vector !
        if(T == 0 && N == 0 && B == 0) {
          continue;
        }

        vcl::vec3 raymarching_dir = vcl::normalize(T * direction + N * dir_right + B * dir_up);
        WorldElement* obstacle = settings.get().raymarcher.raymarch(position, raymarching_dir, obstacles_to_consider);
        if (obstacle != nullptr) {
          // Deal with obstacle : get avoidance_dir and weight
          float d = obstacle->signed_distance(position);
          float w = 1.;
          if (d <= 0) {
            // We're colliding !
            w = 1.;
          }
          else {
            //w = 1.f / d;
            float r = settings.get().raymarcher.max_depth;
            w = (r - d) / r;
            w = w * w;
          }
          //float w = 1.f / ( 1 + obstacle->signed_distance(position))
          accumulated_weighted_avoidance_dirs +=  w * obstacle_avoidance_dir(*obstacle);
        }
      }
    }
  }
  return accumulated_weighted_avoidance_dirs;
}

void Boid::integrate_forces(float dt) {
  /* Symplectic Euler integration */

  // Integrate forces
  // These forces should previously have been updated by the AllBoidsManager
  velocity += applied_forces / settings.get().boid_mass * dt;

  float speed = vcl::norm(velocity);
  direction = velocity / speed;

  // Update normale and binormal directions
  dir_right = normalize(cross(direction, {0,0,1}));
  dir_up = cross(dir_right, direction);

  float clamped_speed = vcl::clamp(speed, settings.get().minSpeed, settings.get().maxSpeed);

  if (clamped_speed != speed) {
    // clamp speed
    velocity = clamped_speed * direction;
  }

  // Integgrate velocity
  position += dt * velocity;
}


