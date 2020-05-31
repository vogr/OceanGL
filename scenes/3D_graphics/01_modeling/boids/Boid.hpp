#pragma once

#include "vcl/vcl.hpp"
#include "BoidSettings.hpp"
#include "BoidHelper.hpp"
#include "../models/WorldElement.h"


class Boid : public WorldElement
{
public:
    std::reference_wrapper<BoidSettings const> settings;
    //vcl::mesh_drawable point;
    vcl::vec3 applied_forces;
    vcl::vec3 position;
    vcl::vec3 direction;
    vcl::vec3 velocity;

    // Cooplete the frame
    vcl::vec3 dir_up;
    vcl::vec3 dir_right;


    Boid* clone() override {return new Boid{*this};};
    vcl::vec3 getPosition() const override {return position; };
    void setPosition(vcl::vec3 p) override {position = p; };

    explicit Boid(BoidSettings const & _settings);
    explicit Boid(BoidSettings const & _settings, vcl::vec3 position_, vcl::vec3 direction_ = {1,0,0});

    vcl::vec3 steer_towards(vcl::vec3 dir, float dt);
    void integrate_forces(float dt);

    vcl::vec3 obstacle_avoidance_dir(WorldElement const & obstacle);
    vcl::vec3 avoid_all_obstacles_weighted_dir(std::vector<std::reference_wrapper<WorldElement>> const & obstacles_to_consider);
};
