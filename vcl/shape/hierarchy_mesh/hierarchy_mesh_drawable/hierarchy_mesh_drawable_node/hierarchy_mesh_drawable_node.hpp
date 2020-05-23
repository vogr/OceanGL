#pragma once

#include "vcl/math/transformation/affine_transform/affine_transform.hpp"
#include "vcl/shape/mesh/mesh_drawable/mesh_drawable.hpp"

namespace vcl {


// A node of the hierarchy
struct hierarchy_mesh_drawable_node {

    hierarchy_mesh_drawable_node() = default;

    hierarchy_mesh_drawable_node(
            mesh_drawable element_arg,
            std::string name_arg,
            std::string name_parent_arg,
            const affine_transform& transform_arg
    );

    // Shortcut constructor to define an directly an initial translation instead of an affine transform
    hierarchy_mesh_drawable_node(
            mesh_drawable element_arg,
            std::string name_arg,
            std::string name_parent_arg,
            const vec3& translation
    );


    mesh_drawable element; // the visual element

    std::string name {"undefined"};        // name of the current node
    std::string name_parent {"global_frame"}; // name of the parent node

    affine_transform transform;        // Local coordinates values for rotation and translation to apply with respect to the parent node frame
    affine_transform global_transform; // Global coordinates values for orientation and translation (supposed to be computed automatically)
};


}
