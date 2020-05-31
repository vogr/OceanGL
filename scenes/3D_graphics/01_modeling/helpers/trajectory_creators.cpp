#include "trajectory_creators.h"

using namespace vcl;

buffer<keyframe> make_shark_trajectory_keyframes() {

  int const N_KEYFRAMES = 10;
  float const r = 30.f;
  buffer<keyframe> trajectory_keyframes;
  float t = 0.;
  for (int i = 0; i < N_KEYFRAMES + 2; i++) {
    auto i_f = static_cast<float>(i);
    auto c = static_cast<float>(std::cos(2. * M_PI * i_f / (N_KEYFRAMES - 1)));
    auto s = static_cast<float>(std::sin(2. * M_PI * i_f / (N_KEYFRAMES - 1)));

    float x = -2.f + r * c;
    float y = -2.5f + r * s;
    float z = 8.f + 15.f * static_cast<float>(std::abs(std::sin(M_PI * i_f / (N_KEYFRAMES - 1))));
    trajectory_keyframes.push_back({{x, y, z}, t});
    t += 1.;
  }
  return trajectory_keyframes;
}


