#include "sampling.h"
#include <glm/geometric.hpp>

glm::vec3 sample_unit_sphere(float u1, float u2) {
  float phi = 2 * M_PI * u1;
  float cos_theta = 1 - 2 * u2;
  float sin_theta = sqrt(1 - cos_theta * cos_theta);
  return glm::normalize(
      glm::vec3(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta));
}

float sample_unit_sphere_pdf() { return 1.0f / (4.0f * M_PI); }

// glm::vec3 sample_unit_disk();

// glm::vec3 sample_unit_disk_pdf();

glm::vec3 sample_hemisphere(float u1, float u2) {
  float phi = 2 * M_PI * u1;
  float cos_theta = u2;
  float sin_theta = sqrt(1 - cos_theta * cos_theta);
  return glm::normalize(
      glm::vec3(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta));
}

float sample_hemisphere_pdf() { return 1.0 / (2 * M_PI); }

glm::vec3 sample_cosine_weighted_hemisphere(float u1, float u2) {
  float phi = 2 * M_PI * u1;
  float cos_theta = sqrt(u2);
  float sin_theta = sqrt(1 - cos_theta * cos_theta);
  return glm::normalize(
      glm::vec3(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta));
}

float sample_cosine_weighted_hemisphere_pdf(float cos_theta) {
  return cos_theta / M_PI;
}