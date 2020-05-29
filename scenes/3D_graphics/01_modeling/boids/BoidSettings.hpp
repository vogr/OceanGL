#pragma once

class BoidSettings{

    public:
        // Settings
        float minSpeed = 2;
        float maxSpeed = 5;
        float perceptionRadius = 2.5f;
        float avoidanceRadius = 1;
        float maxSteerForce = 3;

        float alignWeight = 1;
        float cohesionWeight = 1;
        float seperateWeight = 1;

        float targetWeight = 1;

        //[Header ("Collisions")]
        //LayerMask obstacleMask;
        float boundsRadius = .27f;
        float avoidCollisionWeight = 10;
        float avoidsharkWeight = 5;
        float collisionAvoidDst = 5;
};
