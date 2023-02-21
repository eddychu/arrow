#include "hittable.h"
#include "accel.h"
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
  if (root < r.t_min || root > r.t_max) {
    root = (-b + sqrt_discriminant) / a;
    if (root < r.t_min || root > r.t_max) {
      return false;
    }
  }
  rec.t = root;
  rec.p = r.at(rec.t);
  rec.normal = (rec.p - center) / radius;
  rec.object_id = id();
  return true;
}

bool Scene::hit(const Ray &ray, HitRecord &rec) const {
  if (accel) {
    return accel->hit(ray, rec);
  }
  HitRecord temp_rec;
  bool hit_anything = false;
  float closest_so_far = ray.t_max;
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

void Scene::build_accel() { accel = std::make_unique<BVH>(list); }
