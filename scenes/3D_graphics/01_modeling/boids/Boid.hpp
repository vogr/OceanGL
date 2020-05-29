#pragma once

#include "vcl/vcl.hpp"
#include "BoidSettings.hpp"
#include "BoidHelper.hpp"

class Boid
{
public:
    BoidSettings settings;

    vcl::vec3 position;
    vcl::vec3 velocity;
    vcl::vec3 direction;
    vcl::vec3 center_nearby_flock;
    vcl::vec3 avgFlockHeading;
    vcl::vec3 avgAvoidanceHeading;
    vcl::vec3 avoidSharkHeading;
    vcl::vec3 target;
    float v;
    float radius_of_vision;
    float angle_of_vision;
    float angle_avoidance_fish = M_PI/8;
    int numPerceivedFlockmates;
    float radius_flock;
    BoidHelper boidhelp;


    Boid();
    Boid(vcl::vec3 position_, vcl::vec3 direction_ = {0,0,0}, vcl::vec3 center_nearby_flock_ = {0,0,0}, float v_=0.1,
         float radius_of_vision_ = 0.8, float radius_flock_ = 3.0, float angle_of_vision_ = 3.0*M_PI/4.0);

    bool inCube(vcl::vec3& vector);

    bool free_direction(std::vector<Boid>& all_fish, vcl::vec3 vector);

    void steer_away_from(vcl::vec3& other_position); //function to be used on each boid
    void get_away_from_shark(vcl::vec3& shark_postion);
    void align(vcl::vec3& direction_of_flock);
    void face();
    void avoid_cube();
    void update(std::vector<Boid>& all_fish, float dt);
    vcl::vec3 steer_towards(vcl::vec3 &vector);
    vcl::vec3 ObstacleRays(std::vector<Boid>& all_fish);
    bool IsHeadingForCollision(std::vector<Boid> &all_fish);
};
