#pragma once

#include "vcl/vcl.hpp"

#include "../models/WorldElement.h"

using namespace vcl;

float evaluate_terrain_z(float u, float v);
vec3 evaluate_terrain(float u, float v);
mesh_drawable create_chunk(GLuint texture, int i, int j);

vcl::vec2 terrain_xy_to_uv(vcl::vec2 xy);
vcl::vec2 terrain_uv_to_xy(vcl::vec2 uv);
vcl::vec3 terrain_normal(float u, float v);

