//
// Created by vogier on 02/04/2020.
//

#pragma once

#include <memory>

#include "vcl/vcl.hpp"

class WorldElement {
protected:
    // copying would slice the obbject! Use the clone idiom
    WorldElement(WorldElement const &) = default;
    WorldElement &operator=(WorldElement const &) = default;
    WorldElement(WorldElement &&)= default;
    WorldElement &operator=(WorldElement &&) = default;
public:
  // WorldElement may be used in a polymorphic context :
  virtual ~WorldElement() = default;
  // See https://www.fluentcpp.com/2017/09/08/make-polymorphic-copy-modern-cpp/
  //virtual WorldElement* clone() = 0;

  vcl::affine_transform transform;
  double radius {0.};
  WorldElement() = default;
  WorldElement(vcl::affine_transform t, double r) : transform{t}, radius{r} {};

  virtual void draw(const vcl::camera_scene& camera, GLuint shader) {
    (void)camera;(void)shader; // prevent unused arg warning
    std::cerr << "WARN: Trying to draw WorldElement class. You probably want to draw one of its subclasses.\n";};
};