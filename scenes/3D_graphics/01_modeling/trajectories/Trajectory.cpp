//
// Created by vogier on 21/04/2020.
//

#include "Trajectory.h"
#include <Eigen/Geometry>

using namespace vcl;

/** Function returning the index i such that t \in [v[i].t,v[i+1].t] */
size_t index_at_value(float t, vcl::buffer<keyframe> const& v);

vcl::vec3 linear_interpolation(float t, float t1, float t2, const vcl::vec3& p1, const vcl::vec3& p2);


void Trajectory::init(vcl::buffer<keyframe> _keyframes)
{
    // Initial Keyframe data vector of (position, time)
    keyframes = std::move(_keyframes);
    
    // Set timer bounds
    // You should adapt these extremal values to the type of interpolation
    timer.t_min = keyframes[1].t;                   // first time of the keyframe
    timer.t_max = keyframes[keyframes.size()-2].t;  // last time of the keyframe
    timer.t = timer.t_min;
}

void Trajectory::update() {
  timer.update();
  const float t = timer.t;

  // ********************************************* //
  // Compute interpolated position at time t
  // ********************************************* //
  const int idx = index_at_value(t, keyframes);

  // Preparation of data for the linear interpolation
  // Parameters used to compute the linear interpolation
  keyframe const & k0 = keyframes[idx - 1]; // t_{i-1}
  keyframe const & k1 = keyframes[idx]; // = t_i
  keyframe const & k2 = keyframes[idx + 1]; // = t_{i+1}
  keyframe const & k3 = keyframes[idx + 2]; // = t_{i+2}

  // Update position and TNB
  cardinal_spline_interpolation_update(t, k0, k1, k2, k3, trajectory_tension);

}


size_t index_at_value(float t, vcl::buffer<keyframe> const& v)
{
    const size_t N = v.size();
    assert(v.size()>=2);
    assert(t>=v[0].t);
    assert(t<v[N-1].t);

    size_t k=0;
    while( v[k+1].t<t )
        ++k;
    return k;
}


vec3 linear_interpolation(float t, float t1, float t2, const vec3& p1, const vec3& p2)
{
    const float alpha = (t-t1)/(t2-t1);
    const vec3 p = (1-alpha)*p1 + alpha*p2;

    return p;
}


/**
 *
 * Les positions sont données par une interpolation par spline cardinale.
 *
 * Les rotations sont obtenus de la manière suivante :
 *
 * Chaque keyframe se voit associé une direction verticale ("binormale") par cross product
 * entre les vecteurs vers ses voisins. Exemple : si on a trois keyframes successifs
 *      A ---- B ---- C
 * alors la direction verticale en B est donnée par cross(AB,BC).
 * Pour que cette direction verticale pointe "vers le haut" (z positif), on la retourne
 * si son produit scalaire avec le vecteur unitaire z est négatif.
 *
 * La tangente T en un keyframe est donnée par le vecteur reliant ses voisins (AC pour le point B),
 * c'est le même que pour l'interpolation par spline cardianale.
 *
 * Le dernier vecteur est obtenue par cross product : N = cross(B,T). Avec ces trois vecteurs, on
 * forme un quaternion représentant la rotation à ce keyframe.
 *
 * On obtient les rotations intermédiaires entre les keyframes par slerp dans le domaine
 * des quaternions unitaires.
 *
 */

void Trajectory::cardinal_spline_interpolation_update(float t, keyframe const & k0, keyframe const & k1, keyframe const & k2, keyframe const & k3 , float K) {
  auto t0 = k0.t, t1 = k1.t, t2 = k2.t, t3 = k3.t;
  auto const & p0 = k0.p, p1 = k1.p, p2 = k2.p, p3 = k3.p;

  float const s = (t - t1) / (t2 - t1);
  float const s2 = s*s;
  float const s3 = s*s*s;
  vec3 d1 = 2 * K * (p2 - p0) / (t2 - t0);
  vec3 d2 = 2 * K * (p3 - p1) / (t3 - t1);

  // cardinal spline interpolation of position r
  position =  (2 * s3 - 3 * s2 + 1) * p1 + (s3 - 2*s2 + s) * d1 + (-2*s3 + 3*s2) * p2 + (s3 - s2) * d2;

  /*
  tangent = normalize( (1-s) * d1 + s * d2);
  vec3 T = normalize(tangent - tangent.z);
  normal = {-T.y, T.x, 0};
  binormal = cross(tangent,normal);
  */


  // compute quaternion rotation at 1 and 2 and use spherical interpolation.
  Eigen::Quaternionf q1, q2;

  // Ici on approxime T par interpolation linéaire entre d1 et d2.
  {
    vec3 B = normalize(cross(p0 - p1, p2 - p1));
    if (dot(B,{0,0,1}) < 0) {
      B = -B;
    }
    vec3 const T = normalize(d1 - dot(B, d1) * B);
    vec3 const N = cross(B,T);
    Eigen::Matrix3f m;
    m << T.x, N.x, B.x, T.y, N.y, B.y, T.z, N.z, B.z;
    q1 = Eigen::Quaternionf{m};
  }
  {
    //vec3 const B = normalize(vert2);
    vec3 B = normalize(cross(p1 - p2, p3 - p2));
    if (dot(B,{0,0,1}) < 0) {
      B = -B;
    }
    vec3 const T = normalize(d2 - dot(B, d2) * B);
    vec3 const N = cross(B,T);
    Eigen::Matrix3f m;
    m << T.x, N.x, B.x, T.y, N.y, B.y, T.z, N.z, B.z;
    q2 = Eigen::Quaternionf{m};
  }
  Eigen::Quaternionf q {q1.slerp(s, q2)};

  auto m {q.toRotationMatrix()};

  tangent = {m(0,0), m(1,0), m(2,0)};
  normal = {m(0,1), m(1,1), m(2,1)};
  binormal = {m(0,2), m(1,2), m(2,2)};
}