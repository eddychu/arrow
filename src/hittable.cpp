#include "hittable.h"
#include "accel.h"
#include "bbox.h"
#include <fstream>
#include <glm/ext/scalar_common.hpp>
#include <glm/gtx/intersect.hpp>
#include <iostream>
#include <istream>
#include <memory>
#include <stdexcept>

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
  for (int i = 0; i < m_indices.size(); i += 3) {
    const auto &v0 = m_vertices[m_indices[i]];
    const auto &v1 = m_vertices[m_indices[i + 1]];
    const auto &v2 = m_vertices[m_indices[i + 2]];
    glm::vec3 position;
    if (glm::intersectLineTriangle(ray.origin(), ray.direction(), v0, v1, v2,
                                   position)) {
      auto t = glm::distance(ray.origin(), position);
      if (t < closest_so_far) {
        closest_so_far = t;
        rec.t = t;
        rec.p = position;
        rec.normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        rec.object_id = id();
        is_hit = true;
      }
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

std::unique_ptr<Mesh> Mesh::from_file(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return nullptr;
  }
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> texcoords;
  std::vector<int> indices;
  std::string line;
  while (std::getline(file, line)) {
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
  }
  return std::make_unique<Mesh>(vertices, indices);
}