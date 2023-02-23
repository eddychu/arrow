#include "hittable.h"
#include "accel.h"
#include "bbox.h"
#include "record.h"
#include "sampler.h"
#include <fstream>
#include <glm/ext/scalar_common.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/intersect.hpp>
#include <iostream>
#include <istream>
#include <memory>
#include <stdexcept>

// include assimp headers
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

bool Sphere::hit(const Ray &r, HitRecord &rec) const {
  glm::vec3 oc = r.origin() - center;
  float a = glm::dot(r.direction(), r.direction());
  float b = glm::dot(oc, r.direction());
  float c = glm::dot(oc, oc) - radius * radius;
  float discriminant = b * b - a * c;
  if (discriminant < 0) {
    return false;
  }
  float sqrt_discriminant = sqrt(discriminant);
  float root = (-b - sqrt_discriminant) / a;
  if (root <= r.t_min || root >= r.t_max) {
    root = (-b + sqrt_discriminant) / a;
    if (root <= r.t_min || root >= r.t_max) {
      return false;
    }
  }
  if (root < rec.t) {
    rec.t = root;
    rec.p = r.at(rec.t);
    rec.normal = (rec.p - center) / radius;
    if (glm::dot(r.direction(), rec.normal) > 0) {
      rec.normal = -rec.normal;
    }
    rec.object_id = id();
    return true;
  }
  return false;
}

bool Scene::hit(const Ray &ray, HitRecord &rec) const {
  if (accel) {
    return accel->hit(ray, rec);
  }
  HitRecord temp_rec;
  bool hit_anything = false;
  float closest_so_far = glm::min(rec.t, ray.t_max);
  for (const auto &object : list) {
    if (object->hit(ray, temp_rec) && temp_rec.t < closest_so_far) {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      rec = temp_rec;
    }
  }
  return hit_anything;
}

BBox Scene::bbox() const {
  if (list.empty()) {
    return BBox();
  }

  if (accel) {
    return accel->bbox();
  }

  BBox output_box;
  bool first_box = true;
  for (const auto &object : list) {
    if (first_box) {
      output_box = object->bbox();
      first_box = false;
    } else {
      output_box.expand(object->bbox());
    }
  }
  return output_box;
}

void Scene::build_accel() {
  accel = std::make_unique<BVH>(list);
  accel->build(list);
}

bool Mesh::hit(const Ray &ray, HitRecord &rec) const {
  bool is_hit = false;
  float closest_so_far = glm::min(rec.t, ray.t_max);
  for (size_t i = 0; i < m_indices.size(); i += 3) {
    const auto &v0 = m_vertices[m_indices[i]];
    const auto &v1 = m_vertices[m_indices[i + 1]];
    const auto &v2 = m_vertices[m_indices[i + 2]];

    auto e1 = v1 - v0;
    auto e2 = v2 - v0;
    auto h = glm::cross(ray.direction(), e2);
    auto a = glm::dot(e1, h);
    if (a > -0.00001 && a < 0.00001) {
      continue;
    }
    auto f = 1.0 / a;
    auto s = ray.origin() - v0;
    auto u = f * glm::dot(s, h);
    if (u < 0.0 || u > 1.0) {
      continue;
    }
    auto q = glm::cross(s, e1);
    auto v = f * glm::dot(ray.direction(), q);
    if (v < 0.0 || u + v > 1.0) {
      continue;
    }
    auto t = f * glm::dot(e2, q);
    if (t > ray.t_min && t < closest_so_far) {
      closest_so_far = t;
      rec.t = t;
      rec.p = ray.at(t);
      auto normal = glm::normalize(glm::cross(e1, e2));

      rec.normal = glm::dot(normal, ray.direction()) > 0 ? -normal : normal;

      rec.object_id = id();
      is_hit = true;
    }
  }
  return is_hit;
}

BBox Mesh::bbox() const {
  BBox output_box;
  bool first_box = true;
  for (const auto &v : m_vertices) {
    if (first_box) {
      output_box = BBox(v, v);
      first_box = false;
    } else {
      output_box.expand(v);
    }
  }
  return output_box;
}


glm::vec3 Mesh::sample(const HitRecord& rec, Sampler* sampler) const {

  auto index = static_cast<size_t>(sampler->get_1d() * (m_indices.size() / 3));
  const auto &v0 = m_vertices[m_indices[index]];
  const auto &v1 = m_vertices[m_indices[index + 1]];
  const auto &v2 = m_vertices[m_indices[index + 2]];
  auto u = sampler->get_1d();
  auto v = sampler->get_1d();
  if (u + v > 1) {
    u = 1 - u;
    v = 1 - v;
  }
  auto p = v0 + u * (v1 - v0) + v * (v2 - v0);
  return glm::normalize(p - rec.p);
}


float Mesh::pdf(const HitRecord &rec, const glm::vec3 &dir) const {
  HitRecord test_rec;
  Ray test_ray(rec.p, dir);
  if (!hit(test_ray, test_rec)) {
    return 0.0f;
  }
  auto distance_squared = test_rec.t * test_rec.t;
  auto cosine = glm::abs(glm::dot(dir, test_rec.normal));
  return distance_squared / (cosine * area);
}

std::unique_ptr<Mesh> Mesh::from_file(const std::string &filename) {
  // std::ifstream file(filename);
  // if (!file.is_open()) {
  //   std::cerr << "Failed to open file: " << filename << std::endl;
  //   return nullptr;
  // }
  // std::vector<glm::vec3> vertices;
  // std::vector<glm::vec3> normals;
  // std::vector<glm::vec2> texcoords;
  // std::vector<int> indices;
  // std::string line;
  // while (std::getline(file, line)) {
  // std::istringstream iss(line);
  // std::string type;
  // iss >> type;
  // if (type == "v") {
  //   glm::vec3 v;
  //   iss >> v.x >> v.y >> v.z;
  //   vertices.push_back(v);
  // } else if (type == "vn") {
  //   glm::vec3 n;
  //   iss >> n.x >> n.y >> n.z;
  //   normals.push_back(n);
  // } else if (type == "vt") {
  //   glm::vec2 t;
  //   iss >> t.x >> t.y;
  //   texcoords.push_back(t);
  // } else if (type == "f") {
  //   std::string v1, v2, v3;
  //   iss >> v1 >> v2 >> v3;
  //   indices.push_back(parse_vertex(v1));
  //   indices.push_back(parse_vertex(v2));
  //   indices.push_back(parse_vertex(v3));
  // }
  return nullptr;
}
// return std::make_unique<Mesh>(vertices, indices);