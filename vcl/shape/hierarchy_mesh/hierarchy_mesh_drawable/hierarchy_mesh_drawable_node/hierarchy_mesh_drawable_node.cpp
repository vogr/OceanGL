#include "hierarchy_mesh_drawable_node.hpp"

namespace vcl {

hierarchy_mesh_drawable_node::hierarchy_mesh_drawable_node(mesh_drawable element_arg,
                                                           std::string name_arg,
                                                            std::string name_parent_arg,
                                                           const affine_transform& transform_arg)
    :element(std::move(element_arg)), name(std::move(name_arg)), name_parent(std::move(name_parent_arg)), transform(transform_arg)
{}



hierarchy_mesh_drawable_node::hierarchy_mesh_drawable_node(
                             mesh_drawable element_arg,
                             std::string name_arg,
                             std::string name_parent_arg,
                             const vec3& translation)
    :element(std::move(element_arg)), name(std::move(name_arg)), name_parent(std::move(name_parent_arg)), transform()
{
    transform.translation = translation;
}


}
