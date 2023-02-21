#pragma once
#include <glm/glm.hpp>
#include <limits>
#include <optional>
#include <string>
struct HitRecord {
  float t = std::numeric_limits<float>::max();
  glm::vec3 p;
  glm::vec3 normal;
  std::optional<std::string> object_id;
};
