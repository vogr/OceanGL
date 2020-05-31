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

class FlatBillboard : public DrawableWorldElement {
private:
    FlatBillboardMesh model;
public:
    FlatBillboard* clone() override {return new FlatBillboard(*this);}
    explicit FlatBillboard(FlatBillboardMesh m, affine_transform t, float radius) : DrawableWorldElement{radius}, model{std::move(m)} {model.billboard.uniform.transform = t;};
    void draw(const vcl::camera_scene& camera, const light_animation_data & light_data, DrawType draw_type) override;
    vcl::vec3 getPosition() const override {return model.billboard.uniform.transform.translation; };
    void setPosition(vcl::vec3 p) override {model.billboard.uniform.transform.translation = p; };
};

class CrossBillboard : public DrawableWorldElement {
private:
    CrossBillboardMesh model;
public:
    affine_transform transform;
    CrossBillboard* clone() override {return new CrossBillboard(*this);}
    explicit CrossBillboard(CrossBillboardMesh m, affine_transform t, float radius) : DrawableWorldElement{radius}, model{std::move(m)}, transform{t} {};
    void draw(const vcl::camera_scene& camera, const light_animation_data & light_data, DrawType draw_type) override;
    vcl::vec3 getPosition() const override {return transform.translation; };
    void setPosition(vcl::vec3 p) override {transform.translation = p; };
};