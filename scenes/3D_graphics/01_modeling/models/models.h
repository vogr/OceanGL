#pragma once

#include "vcl/vcl.hpp"
#include "WorldElement.h"
#include <memory>

using namespace vcl;

mesh create_cylinder(float radius, float height);
mesh create_cone(float radius, float height, float z_offset);