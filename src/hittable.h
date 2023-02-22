#pragma once
#include "bbox.h"
#include "material.h"
#include "ray.h"
#include "record.h"
#include "uuid.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

class Hittable {
public:
  virtual ~Hittable() {}
  virtual bool hit(const Ray &r, HitRecord &rec) const = 0;
  virtual BBox bbox() const = 0;
  virtual std::optional<std::string> id() const { return std::nullopt; }
  virtual std::shared_ptr<Material> material() const { return nullptr; }
};

class Sphere : public Hittable {
public:
  Sphere() : m_id(uuid::generate_uuid_v4()) {}
  Sphere(glm::vec3 cen, float r, std::shared_ptr<Material> mat)
      : center(cen), radius(r), m_id(uuid::generate_uuid_v4()),
        m_material(mat){};
  virtual bool hit(const Ray &r, HitRecord &rec) const;
  virtual BBox bbox() const override {
    return BBox(center - glm::vec3(radius), center + glm::vec3(radius));
  }

  virtual std::shared_ptr<Material> material() const override {
    return m_material;
  }
  virtual std::optional<std::string> id() const override { return m_id; }
  glm::vec3 center;
  float radius;
  std::string m_id;
  std::shared_ptr<Material> m_material = nullptr;
};

class Scene : public Hittable {
public:
  Scene() {}

  void build_accel();

  void add(std::unique_ptr<Hittable> h) { list.push_back(std::move(h)); }
  virtual bool hit(const Ray &r, HitRecord &rec) const override;

  virtual BBox bbox() const override;

  const std::unique_ptr<Hittable> &get(const std::string &id) const {
    for (const auto &h : list) {
      if (h->id() && h->id() == id) {
        return h;
      }
    }
    throw std::runtime_error("No hittable with id " + id);
  }

  std::vector<std::unique_ptr<Hittable>> list;
  std::unique_ptr<class Accel> accel = nullptr;
};

class Mesh : public Hittable {
public:
  Mesh(const std::vector<glm::vec3> &vertices, const std::vector<int> &indices,
       std::shared_ptr<Material> mat)
      : m_vertices(vertices), m_indices(indices), m_material(mat) {}

  virtual bool hit(const Ray &r, HitRecord &rec) const override;

  virtual BBox bbox() const override;

  static std::unique_ptr<Mesh> from_file(const std::string &filename);

  virtual std::optional<std::string> id() const override {
      return m_id;
  }

  virtual std::shared_ptr<Material> material() const override {
    return m_material;
  }

private:
  std::vector<glm::vec3> m_vertices;
  std::vector<int> m_indices;
  std::shared_ptr<Material> m_material = nullptr;
  std::string m_id = uuid::generate_uuid_v4();
};
