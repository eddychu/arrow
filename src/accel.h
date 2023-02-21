#pragma once
#include "bbox.h"
#include "record.h"
#include <algorithm>
#include <memory>
#include <numeric>
#include <vector>
class Hittable;
class Accel {
public:
  virtual void build(const std::vector<std::unique_ptr<Hittable>> &list) = 0;
  virtual bool hit(const Ray &ray, HitRecord &record) const = 0;
  virtual BBox bbox() const = 0;
};

struct BVHNode {
  int left;
  int right;
  BBox bbox;
  // this is only used for leaf nodes
  std::vector<int> primitives;
};

class BVH : public Accel {
public:
  BVH(const std::vector<std::unique_ptr<Hittable>> &list);

  virtual void
  build(const std::vector<std::unique_ptr<Hittable>> &list) override;

  virtual BBox bbox() const override { return nodes[root].bbox; }

  virtual bool hit(const Ray &ray, HitRecord &record) const override;

private:
  int build_recursive(std::vector<int> &indices);

  bool hit_node(const Ray &ray, HitRecord &record, int node) const;

private:
  std::vector<BVHNode> nodes;
  size_t root;
  const std::vector<std::unique_ptr<Hittable>> &list;

  static size_t MAX_PRIMITIVES_PER_LEAF;
};
