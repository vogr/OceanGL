#include "Boid.hpp"
#include <iostream>

bool inCube(vcl::vec3 vector){
    float L = 40.f;
    return
       (std::abs(vector[0]) < L) &&
       (std::abs(vector[1]) < L) &&
       (vector[2] > 5) && (vector[2] < 30);
}


/** This part is for defining the boids' behaviour depending on the list of nearby boids*/
Boid::Boid(){
    //point = mesh_primitive_sphere(0.2);
    position = {0.0,0.0,0.0};
    center_nearby_flock = {0.0,0.0,0.0};
    direction = {1.0,0.0,0.0};
}

Boid::Boid(vcl::vec3 position_, vcl::vec3 direction_, vcl::vec3 center_nearby_flock_,  float v_, float radius_of_vision_, float radius_flock_,
           float angle_of_vision_){
    position = position_;
   // point = mesh_primitive_sphere(0.2,position);
    center_nearby_flock = center_nearby_flock_;
    direction = vcl::normalize(direction_);
    velocity=v_*direction;
    radius_of_vision = radius_of_vision_;
    radius_flock = radius_flock_;
    angle_of_vision = angle_of_vision_;
    target = (position + direction);
}


void Boid::steer_away_from(Boid& other){
    vcl::vec3 pointer_to_other = other.position - this->position;
    /* d : direction
     * p : pointer to other
     * q : symetric to p by d ; q = 2d-p
     * nd: new direction ; nd = (d+q)/2 = (3d-p)/2
     */
    /*
    this->direction = (3*this->direction - pointer_to_other)/2.0;
    this->direction = vcl::normalize(this->direction);
    */
    this->avgAvoidanceHeading+=vcl::normalize(pointer_to_other);
}

void Boid::align(vcl::vec3& direction_of_flock){
    this->direction =  (4.0*this->direction + direction_of_flock);
    this->direction = vcl::normalize(this->direction);
}

void Boid::face(){
    vcl::vec3 pointer_to_center = this->center_nearby_flock - this->position;
    /* d : direction
     * p : pointer to center
     * q : symetric to p by d ; q = 2d-p
     * nd: new direction ; nd = (d+q)/2 = (3d-p)/2
     */
    this->direction = (3*this->direction - pointer_to_center)/2.0;
    this->direction = vcl::normalize(this->direction);
}

/*
void Boid::avoid_cube(){


    float dist = distance_cube();
    if(dist < 4){
        this->direction = -this->direction;
        std::cout<<"avoiding cube"<<std::endl;
    }


    vcl::vec3 zero = {0,0,0};
    float dist = distance_cube();
    float avoiding_acceleration= 0.7;
    std::cout<<"dist to cube = "<< dist << std::endl;
    if(dist < 4){
        std::cout<<"entered because dist to cube = "<< dist << std::endl;
        if(this->avoid_cube_direction.equal(zero)){
            vcl::vec3 orthogonal_dir = vcl::normalize(vcl::vec3(this->direction[1], -this->direction[0], -this->direction[2]));
            this->avoid_cube_direction = avoiding_acceleration * orthogonal_dir;
        }
        this->direction += this->avoid_cube_direction;
        vcl::vec3 direction_avoid_cube= vcl::normalize(this->avoid_cube_direction);
        if(vcl::normalize(this->direction).equal(direction_avoid_cube))  this->avoid_cube_direction=zero;
    }
    else if(!this->avoid_cube_direction.equal(zero))
        this->avoid_cube_direction = zero;

}
*/

vcl::vec3 Boid::steer_towards(vcl::vec3 vector){

    vcl::vec3 v = vcl::normalize(vector) * this->settings.maxSpeed - this->velocity;

    double const n = norm(v);
    if(n>settings.maxSteerForce)
        v=settings.maxSteerForce * (v/n);
    return v;
}


vcl::vec3 Boid::ObstacleRays(){
    //only works in a cube
    auto & rayDirections = boidhelp.directions;

    for (int i = 0; i < boidhelp.numViewDirections; i++) {
        //transform direction in global parameters
        vcl::vec3 dir = rotation_between_vector_mat3({0,0,1}, direction)*rayDirections[i];
        //std::cout << "new direction should be this? : " << dir << std::endl;

        /*
        auto objectif =  position + dir;    //ray
        if (!Physics.SphereCast (ray, settings.boundsRadius, settings.collisionAvoidDst, settings.obstacleMask)) {
            return dir;
        }
        */
        if(inCube(position + (settings.collisionAvoidDst+settings.boundsRadius)*normalize(dir))){
            return dir;
        }
    }

    return direction;
}

bool Boid::IsHeadingForCollision(){
    if(inCube(position + (settings.collisionAvoidDst-settings.boundsRadius)*normalize(direction)))
       return false;
    return true;
}

void Boid::update(std::vector<Boid>& all_fish, float dt){
    //see all fish and determine if in nearby flock
    numPerceivedFlockmates = 0;
    this->center_nearby_flock = {0.0,0.0,0.0};
    this->avgFlockHeading = {0.0,0.0,0.0};
    vcl::vec3 direction_of_flock = this->direction;
    for(auto & fish : all_fish){
        vcl::vec3 pointer_to_fish = fish.position - this->position;
        float distance = vcl::norm(pointer_to_fish);

        if(distance == 0) {
          //
        }
        else if(distance < this->radius_of_vision){
            float cos_angle = vcl::dot(this->direction, pointer_to_fish) / (vcl::norm(this->direction)*vcl::norm(pointer_to_fish));
            float angle = std::acos(cos_angle);
            if(angle < this->angle_of_vision){
                this->steer_away_from(fish);
            }
        }

        if(distance < this->radius_flock){
            numPerceivedFlockmates++;
            avgFlockHeading += fish.direction;
            this->center_nearby_flock += fish.position;
        }
    }

    if(numPerceivedFlockmates>0){
        direction_of_flock = vcl::normalize(direction_of_flock);
        this->center_nearby_flock = this->center_nearby_flock/numPerceivedFlockmates;
    }
    else
        this->center_nearby_flock = this->position;



    vcl::vec3 acceleration = {0,0,0};

    vcl::vec3 offsetToTarget = target - position;
    acceleration+= steer_towards(offsetToTarget);


    if (numPerceivedFlockmates != 0) {
        center_nearby_flock /= numPerceivedFlockmates;

        vcl::vec3 offsetToFlockmatesCentre = (center_nearby_flock - position);

        auto alignmentForce = steer_towards(avgFlockHeading) * settings.alignWeight;
        auto cohesionForce = steer_towards(offsetToFlockmatesCentre) * settings.cohesionWeight;
        auto seperationForce = steer_towards(avgAvoidanceHeading) * settings.seperateWeight;

        acceleration += alignmentForce;
        acceleration += cohesionForce;
        acceleration += seperationForce;
     }

    if (IsHeadingForCollision()) {
        vcl::vec3 collisionAvoidDir = ObstacleRays();
        vcl::vec3 collisionAvoidForce = steer_towards(collisionAvoidDir) * settings.avoidCollisionWeight;
        acceleration += collisionAvoidForce;
    }


    velocity += acceleration * dt;

    float speed = vcl::norm(velocity);
    vcl::vec3 dir = velocity / speed;

    speed = vcl::clamp(speed, settings.minSpeed, settings.maxSpeed);
    velocity = dir * speed;

    this->position += velocity * dt;
    this->direction = dir;
    target = position + settings.maxSpeed*direction;
}
