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
  virtual WorldElement* clone() = 0;

  float radius {0.};
  WorldElement() = default;
  explicit WorldElement(float r) : radius{r} {};

  // Default signed distance given by the englobing sphere, might be overriden.
  virtual float signed_distance(vcl::vec3 p) const;
  // A WorldElement should simply have a position and a radius.
  virtual vcl::vec3 getPosition() const = 0;
  virtual void setPosition(vcl::vec3 p) = 0;

};

class DrawableWorldElement : public WorldElement {
public:
    DrawableWorldElement() = default;
    virtual DrawableWorldElement* clone() = 0;

    explicit DrawableWorldElement(float r) : WorldElement{r} {};
    DrawableWorldElement(DrawableWorldElement const &) = default;
    DrawableWorldElement &operator=(DrawableWorldElement const &) = default;
    DrawableWorldElement(DrawableWorldElement &&)= default;
    DrawableWorldElement &operator=(DrawableWorldElement &&) = default;
    virtual void draw(const vcl::camera_scene& camera, const vcl::light_animation_data & light_data, vcl::DrawType draw_type) = 0;
};
