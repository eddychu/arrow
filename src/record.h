#pragma once
#include <glm/glm.hpp>
#include <optional>
#include <string>
struct HitRecord {
  float t;
  glm::vec3 p;
  glm::vec3 normal;
  std::optional<std::string> object_id;
};
