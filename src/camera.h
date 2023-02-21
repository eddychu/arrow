#pragma once
#include "ray.h"
#include "transform.h"
#include <glm/trigonometric.hpp>
class Camera {
public:
  Camera(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up,
         float fov, float aspect)
      : m_transform(Transform::lookAt(eye, center, up)), m_fov(fov),
        m_aspect(aspect) {}

  Ray get_ray(float s, float t) const {
    auto theta = glm::radians(m_fov);
    auto half_height = glm::tan(theta / 2);
    auto half_width = m_aspect * half_height;
    auto origin = m_transform.point_to_world(glm::vec3(0, 0, 0));
    auto dir = m_transform.vector_to_world(
        glm::vec3(s * half_width, t * half_height, -1));
    return Ray(origin, dir);
  }

private:
  Transform m_transform;
  float m_fov;
  float m_aspect;
};