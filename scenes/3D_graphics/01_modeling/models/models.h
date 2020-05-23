#pragma once

#include "vcl/vcl.hpp"
#include "WorldElement.h"
#include <memory>

using namespace vcl;

mesh create_cylinder(float radius, float height);
mesh create_cone(float radius, float height, float z_offset);
mesh create_tree_foliage(float radius, float height, float z_offset);


class MushroomMesh {
public:
    explicit MushroomMesh(GLuint texture_id);
    vcl::mesh_drawable foot;
    vcl::mesh_drawable head;
};

class TreeElement : public WorldElement {
private:
    std::shared_ptr<hierarchy_mesh_drawable> tree;
public:
    //TreeElement* clone() override {return new TreeElement(*this);}
    static hierarchy_mesh_drawable create_tree_model(GLuint texture_id);
    explicit TreeElement(std::shared_ptr<hierarchy_mesh_drawable> tr, affine_transform t, double radius) : WorldElement{t, radius}, tree{std::move(tr)} {};
    void draw(const camera_scene& camera, const light_animation_data & light_data, DrawType draw_type);
};

class MushroomElement : public WorldElement {
private:
    MushroomMesh mushroom;
public:
    //MushroomElement* clone() override {return new MushroomElement(*this);}
    explicit MushroomElement(MushroomMesh m, affine_transform t, double radius) : WorldElement{t, radius}, mushroom{std::move(m)} {};
    void draw(const camera_scene& camera, GLuint shader) override;
};
