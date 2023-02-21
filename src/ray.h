#pragma once
#include <glm/glm.hpp>
#include <limits>
class Ray {
public:
  Ray() {}
  Ray(const glm::vec3 &a, const glm::vec3 &b) {
    o = a;
    d = b;
  }
  glm::vec3 origin() const { return o; }
  glm::vec3 direction() const { return d; }
  glm::vec3 at(float t) const { return o + t * d; }

  glm::vec3 o;
  glm::vec3 d;
  float t_min = 0.0001;
  float t_max = std::numeric_limits<float>::max();
};