#pragma once

#include "vcl/vcl.hpp"
#include "WorldElement.h"

#include <memory>

using namespace vcl;

/* TODO:
 * TODO: Unify FlatBillboardMesh and CrossBillboardMesh
 */

class FlatBillboardMesh {
public:
    mesh_drawable billboard;
    explicit FlatBillboardMesh(GLuint texture_id);
};

class CrossBillboardMesh {
public:
    mesh_drawable billboard[4];
    explicit CrossBillboardMesh(GLuint texture_id);
    void set_texture_id(GLuint tex_id);
};

class FlatBillboard : public WorldElement {
private:
    FlatBillboardMesh model;
public:
    FlatBillboard* clone() override {return new FlatBillboard(*this);}
    explicit FlatBillboard(FlatBillboardMesh m, affine_transform t, double radius) : WorldElement{t, radius}, model{std::move(m)} {};
    void draw(const vcl::camera_scene& camera, const light_animation_data & light_data, DrawType draw_type) override;
};

class CrossBillboard : public WorldElement {
private:
    CrossBillboardMesh model;
public:
    CrossBillboard* clone() override {return new CrossBillboard(*this);}
    explicit CrossBillboard(CrossBillboardMesh m, affine_transform t, double radius) : WorldElement{t, radius}, model{std::move(m)} {};
    void draw(const vcl::camera_scene& camera, const light_animation_data & light_data, DrawType draw_type) override;
};