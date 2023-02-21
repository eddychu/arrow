#include "accel.h"
#include "hittable.h"
#include <limits.h>
size_t BVH::MAX_PRIMITIVES_PER_LEAF = 4;
BVH::BVH(const std::vector<std::unique_ptr<Hittable>> &list) : list(list) {}

void BVH::build(const std::vector<std::unique_ptr<Hittable>> &list) {
  nodes.clear();
  root = -1;
  std::vector<int> indices(list.size());
  std::iota(indices.begin(), indices.end(), 0);
  root = build_recursive(indices);
}

int BVH::build_recursive(std::vector<int> &indices) {
  BBox box;
  for (auto i : indices) {
    box.expand(list[i]->bbox());
  }
  if (indices.size() <= MAX_PRIMITIVES_PER_LEAF) {
    int node_index = nodes.size();
    nodes.push_back(BVHNode{-1, -1, box, indices});
    return node_index;
  }
  int axis = box.max_extent();
  std::sort(indices.begin(), indices.end(), [this, axis](int a, int b) {
    return list[a]->bbox().center()[axis] < list[b]->bbox().center()[axis];
  });
  int mid = indices.size() / 2;
  std::vector<int> left_indices(indices.begin(), indices.begin() + mid);
  std::vector<int> right_indices(indices.begin() + mid, indices.end());
  auto left = build_recursive(left_indices);
  auto right = build_recursive(right_indices);
  auto node = BVHNode{left, right, box, {}};

  int node_index = nodes.size();
  nodes.push_back(node);
  return node_index;
}

bool BVH::hit(const Ray &ray, HitRecord &record) const {
  return hit_node(ray, record, root);
}

bool BVH::hit_node(const Ray &ray, HitRecord &record, int node) const {
  if (!nodes[node].bbox.hit(ray)) {
    return false;
  }
  HitRecord temp;
  bool hit = false;
  float closest = std::min(record.t, ray.t_max);
  if (nodes[node].left < 0 && nodes[node].right < 0) {
    for (const auto &primitive : nodes[node].primitives) {
      const auto &object = list[primitive];
      if (object->hit(ray, temp) && temp.t < closest) {
        closest = temp.t;
        record = temp;
        hit = true;
      }
    }
  }

  if (nodes[node].left >= 0) {
    hit |= hit_node(ray, record, nodes[node].left);
  }
  if (nodes[node].right >= 0) {
    hit |= hit_node(ray, record, nodes[node].right);
  }

  return hit;
}
