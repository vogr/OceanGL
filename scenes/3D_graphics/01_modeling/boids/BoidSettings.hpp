#pragma once

#include "../raymarcher/Raymarcher.hpp"

class BoidSettings{

    public:
        // Vision
        float radius_of_vision = 30.f;
        float min_cos_in_vision = -0.8f;


        // Force and speed
        float boid_mass = 1.f;
        float minSpeed = 2.f;
        //float maxSpeed = 30.f;
        float maxSpeed = 25.f;
        float maxSteerForce = 6.f;

        // Weights of the different rules
        float alignmentWeight = 1.4f;
        float cohesionWeight = 1.8f;
        float seperationWeight = 2.3f;

        // 4th rule : stay within slice [z_min, z_max] (approximately)
        float stayInBoundsWeight = 1.f;
        float z_min = 12.f;
        float z_max = 50.f;

        //[Header ("Collisions")]
        //LayerMask obstacleMask;
        float boundsRadius = .27f;
        float avoidCollisionWeight = 10.f;
        float collisionAvoidDst = 5;

        float horizontality_weight = 1.f;

        Raymarcher raymarcher;
};
