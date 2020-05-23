#pragma once

#include "vcl/shape/mesh/mesh_drawable/mesh_drawable.hpp"
#include "hierarchy_mesh_drawable_node/hierarchy_mesh_drawable_node.hpp"
#include "hierarchy_mesh_drawable_display_skeleton/hierarchy_mesh_drawable_display_skeleton.hpp"

#include <map>

namespace vcl{

struct hierarchy_mesh_drawable
{

    std::map<std::string, int> name_map;
    std::vector<hierarchy_mesh_drawable_node> elements;


    // Add new node to the hierarchy
    // Note: Parent node is expected to be already present in the hierarchy
    // The name of each node must be unique in the hierarchy
    void add(const hierarchy_mesh_drawable_node& node);

    // Shortcut to add a new node
    void add(
        mesh_drawable element,
        std::string name,
        std::string name_parent="global_frame",
        affine_transform const & transform= affine_transform()
    );

    // Shortcut to add directly an initial translation instead of an affine transform
    void add(
      mesh_drawable element,
      std::string name,
      std::string name_parent,
      const vec3& translation
    );

    // Get node by name
    hierarchy_mesh_drawable_node& operator[](const std::string& name);
    // Get node by name
    const hierarchy_mesh_drawable_node& operator[](const std::string& name) const;


    // Fill global coordinates of the nodes given the local one
    void update_local_to_global_coordinates();

    /** Set the same shader for all elements of the hierarchy */
    void set_shader_for_all_elements(GLuint shader);




};

/** Display all the elements of the hierarchy
    Assume that the hierarchy has updated global coordinates
    Always draw using the default shader.
*/
//void draw(const hierarchy_mesh_drawable& hierarchy, const camera_scene& camera);
void draw(const hierarchy_mesh_drawable& hierarchy, const camera_scene& camera, light_animation_data const & light_data, DrawType draw_type);
}
