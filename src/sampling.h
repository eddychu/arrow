#pragma once

#include <glm/glm.hpp>

glm::vec3 sample_unit_sphere(float u1, float u2);

float sample_unit_sphere_pdf();

// glm::vec3 sample_unit_disk();

// glm::vec3 sample_unit_disk_pdf();

glm::vec3 sample_hemisphere(float u1, float u2);

float sample_hemisphere_pdf();

glm::vec3 sample_cosine_weighted_hemisphere(float u1, float u2);

float sample_cosine_weighted_hemisphere_pdf();
