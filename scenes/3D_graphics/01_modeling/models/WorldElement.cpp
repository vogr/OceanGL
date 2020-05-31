//
// Created by vogier on 02/04/2020.
//

#include "WorldElement.h"


float WorldElement::signed_distance(vcl::vec3 p) const {
  auto pos = getPosition();
  return vcl::norm(p - pos) - radius;
}


