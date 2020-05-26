#pragma once

#include "vcl/vcl.hpp"
#include "WorldElement.h"
#include <memory>

using namespace vcl;

mesh create_cylinder(float radius, float height);
mesh create_cone(float radius, float height, float z_offset);
mesh create_tree_foliage(float radius, float height, float z_offset);

class TreeElement : public WorldElement {
private:
    std::shared_ptr<hierarchy_mesh_drawable> tree;
public:
    TreeElement* clone() override {return new TreeElement(*this);}
    static hierarchy_mesh_drawable create_tree_model(GLuint texture_id);
    explicit TreeElement(std::shared_ptr<hierarchy_mesh_drawable> tr, affine_transform t, double radius) : WorldElement{t, radius}, tree{std::move(tr)} {};
    void draw(const camera_scene& camera, const light_animation_data & light_data, DrawType draw_type) override;
};