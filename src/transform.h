#pragma once

#include <glm/ext.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
using namespace glm;
class Transform {
public:
  static Transform lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) {
    auto inv = glm::lookAt(eye, center, up);
    auto m = inverse(inv);
    return Transform(m, inv);
  }

  Transform(const mat4 &matrix)
      : m_matrix(matrix), m_inverse(inverse(matrix)) {}
  Transform(const mat4 &matrix, const mat4 &inverse)
      : m_matrix(matrix), m_inverse(inverse) {}
  vec3 vector_to_world(const vec3 &v) const {
    return vec3(m_matrix * vec4(v, 0.0f));
  }
  vec3 point_to_world(const vec3 &p) const {
    return vec3(m_matrix * vec4(p, 1.0f));
  }

  vec3 vector_to_local(const vec3 &v) const {
    return vec3(m_inverse * vec4(v, 0.0f));
  }
  vec3 point_to_local(const vec3 &p) const {
    return vec3(m_inverse * vec4(p, 1.0f));
  }

private:
  mat4 m_matrix;
  mat4 m_inverse;
};
