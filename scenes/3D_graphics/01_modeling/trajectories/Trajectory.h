//
// Created by vogier on 21/04/2020.
//

#pragma once

#include "vcl/vcl.hpp"

// Store a vec3 (p) + time (t)
class keyframe {
public:
    keyframe() = default;
    keyframe(vcl::vec3 _p, float _t) : p{_p}, t{_t} {}

    vcl::vec3 p; // position
    float t {0.f};     // time
};

// TODO : move the mesh_drawables used in trajectory in a struct. Keep a shared pointer
// TODO : to this struct in every Trajectory.

class Trajectory {
public:

    void init(vcl::buffer<keyframe> _keyframes);
    void update();

    vcl::timer_interval timer;

    vcl::buffer<keyframe> keyframes; // Given (position,time)

    void cardinal_spline_interpolation_update(float t, keyframe const & k0, keyframe const & k1, keyframe const & k2, keyframe const & k3 , float K);
    vcl::vec3 position;
    // TNB frame
    vcl::vec3 tangent;
    vcl::vec3 normal;
    vcl::vec3 binormal;

    float trajectory_tension {0.5};
};