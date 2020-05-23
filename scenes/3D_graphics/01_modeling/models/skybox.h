//
// Created by vogier on 02/04/2020.
//
#pragma once

#include <memory>

#include "vcl/vcl.hpp"
#include "WorldElement.h"

typedef std::array<vcl::mesh_drawable,6> skybox_models;

skybox_models build_skybox();


class Skybox : public WorldElement {
private:
    skybox_models models;
public:
    Skybox() = default;
    //Skybox* clone() override {return new Skybox(*this);}
    explicit Skybox(skybox_models m) : models{std::move(m)} {};
    void draw(const vcl::camera_scene& camera, GLuint shader) override;
};