#include "hittable.h"
#include "accel.h"
#include "bbox.h"
#include "record.h"
#include "sampler.h"
#include "sampling.h"
#include <cmath>
#include <fstream>
#include <glm/ext/scalar_common.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/intersect.hpp>
#include <iostream>
#include <istream>
#include <memory>
#include <stdexcept>
// include assimp headers
// #include <assimp/Importer.hpp>
// #include <assimp/postprocess.h>
// #include <assimp/scene.h>
bool Sphere::hit(const Ray &r, HitRecord &rec) const
{
  glm::vec3 oc = r.origin() - center;
  float a = glm::dot(r.direction(), r.direction());
  float b = glm::dot(oc, r.direction());
  float c = glm::dot(oc, oc) - radius * radius;
  float discriminant = b * b - a * c;
  if (discriminant < 0)
  {
    return false;
  }
  float sqrt_discriminant = sqrt(discriminant);
  float root = (-b - sqrt_discriminant) / a;
  if (root <= r.t_min || root >= r.t_max)
  {
    root = (-b + sqrt_discriminant) / a;
    if (root <= r.t_min || root >= r.t_max)
    {
      return false;
    }
  }
  if (root < rec.t)
  {
    rec.t = root;
    rec.p = r.at(rec.t);
    rec.normal = (rec.p - center) / radius;
    if (glm::dot(r.direction(), rec.normal) > 0)
    {
      rec.normal = -rec.normal;
    }
    rec.object_id = id();
    return true;
  }
  return false;
}

glm::vec3 Sphere::sample(const HitRecord &rec, Sampler *sampler) const
{
  glm::vec3 direction = center - rec.p;
  auto distance_squared = glm::dot(direction, direction);
  // create a new coordinate system based on the normal
  auto w = direction;
  auto a =
      w.x > 0.9f ? glm::vec3(0.0, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
  auto v = glm::normalize(glm::cross(w, a));
  auto u = glm::cross(w, v);
  // calculate the max cosine of the cone.
  auto cos_theta_max = sqrt(1 - radius * radius / distance_squared);
  auto dir = sample_cone(sampler->get_1d(), sampler->get_1d(), cos_theta_max);
  return glm::normalize(dir.x * u + dir.y * v + dir.z * w);
}

float Sphere::pdf(const HitRecord &rec, const glm::vec3 &wi) const
{
  HitRecord test_rec;
  if (!hit(Ray(rec.p, wi), test_rec))
  {
    return 0.0f;
  }
  auto len = glm::length(center - rec.p);
  auto cos_theta_max = sqrt(1 - radius * radius / len * len);
  float solid_angle = 2 * glm::pi<float>() * (1 - cos_theta_max);
  return 1 / solid_angle;
}

bool Scene::hit(const Ray &ray, HitRecord &rec) const
{
  if (accel)
  {
    return accel->hit(ray, rec);
  }
  HitRecord temp_rec;
  bool hit_anything = false;
  float closest_so_far = glm::min(rec.t, ray.t_max);
  for (const auto &object : list)
  {
    if (object->hit(ray, temp_rec) && temp_rec.t < closest_so_far)
    {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      rec = temp_rec;
    }
  }
  return hit_anything;
}

std::unique_ptr<Scene> Scene::from_file(const std::string &filename)
{
  // Assimp::Importer importer;
  // const aiScene *scene = importer.ReadFile(
  //     filename, aiProcess_Triangulate | aiProcess_GenNormals |
  //                   aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
  // if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
  //     !scene->mRootNode)
  // {
  //   std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
  //   return nullptr;
  // }
  // auto output = std::make_unique<Scene>();
  // output->list.reserve(scene->mNumMeshes);
  // for (unsigned int i = 0; i < scene->mNumMeshes; i++)
  // {
  //   auto mesh = scene->mMeshes[i];
  //   std::vector<glm::vec3> vertices;
  //   std::vector<glm::vec3> normals;
  //   std::vector<glm::vec2> uvs;
  //   std::vector<glm::vec3> tangents;
  //   std::vector<glm::vec3> bitangents;
  //   vertices.reserve(mesh->mNumVertices);
  //   normals.reserve(mesh->mNumVertices);
  //   uvs.reserve(mesh->mNumVertices);
  //   tangents.reserve(mesh->mNumVertices);
  //   bitangents.reserve(mesh->mNumVertices);
  //   for (unsigned int j = 0; j < mesh->mNumVertices; j++)
  //   {
  //     vertices.push_back(glm::vec3(mesh->mVertices[j].x, mesh->mVertices[j].y,
  //                                  mesh->mVertices[j].z));
  //     normals.push_back(glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y,
  //                                 mesh->mNormals[j].z));
  //     if (mesh->mTextureCoords[0])
  //     {
  //       uvs.push_back(glm::vec2(mesh->mTextureCoords[0][j].x,
  //                               mesh->mTextureCoords[0][j].y));
  //     }
  //     else
  //     {
  //       uvs.push_back(glm::vec2(0.0f, 0.0f));
  //     }
  //     if (mesh->mTangents)
  //     {
  //       tangents.push_back(glm::vec3(mesh->mTangents[j].x, mesh->mTangents[j].y,
  //                                    mesh->mTangents[j].z));
  //     }
  //     else
  //     {
  //       tangents.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
  //     }
  //     if (mesh->mBitangents)
  //     {
  //       bitangents.push_back(glm::vec3(mesh->mBitangents[j].x,
  //                                      mesh->mBitangents[j].y,
  //                                      mesh->mBitangents[j].z));
  //     }
  //     else
  //     {
  //       bitangents.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
  //     }
  //   }
  //   std::vector<unsigned int> indices;
  //   indices.reserve(mesh->mNumFaces * 3);
  //   for (unsigned int j = 0; j < mesh->mNumFaces; j++)
  //   {
  //     auto face = mesh->mFaces[j];
  //     for (unsigned int k = 0; k < face.mNumIndices; k++)
  //     {
  //       indices.push_back(face.mIndices[k]);
  //     }
  //   }
  //   auto mateiral = std::make_shared<Lambertian>(glm::vec3(0.73f, 0.73f, 0.73f));
  //   auto object = std::make_unique<Mesh>(vertices, indices,
  //                                        mateiral);
  //   output->add(std::move(object));
  // }
  // return output;
}

BBox Scene::bbox() const
{
  if (list.empty())
  {
    return BBox();
  }

  if (accel)
  {
    return accel->bbox();
  }

  BBox output_box;
  bool first_box = true;
  for (const auto &object : list)
  {
    if (first_box)
    {
      output_box = object->bbox();
      first_box = false;
    }
    else
    {
      output_box.expand(object->bbox());
    }
  }
  return output_box;
}

void Scene::build()
{
  if (!accel)
  {
    build_accel();
  }

  // find all ids of lights
  for (const auto &h : list)
  {
    if (glm::length(h->material()->emitted()) > 0.0f)
    {
      lights.push_back(h->id().value());
    }
  }
}

void Scene::build_accel()
{
  accel = std::make_unique<BVH>(list);
  accel->build(list);
}

bool Mesh::hit(const Ray &ray, HitRecord &rec) const
{
  bool is_hit = false;
  float closest_so_far = glm::min(rec.t, ray.t_max);
  for (size_t i = 0; i < m_indices.size(); i += 3)
  {
    const auto &v0 = m_vertices[m_indices[i]];
    const auto &v1 = m_vertices[m_indices[i + 1]];
    const auto &v2 = m_vertices[m_indices[i + 2]];

    auto e1 = v1 - v0;
    auto e2 = v2 - v0;
    auto h = glm::cross(ray.direction(), e2);
    auto a = glm::dot(e1, h);
    if (a > -0.00001 && a < 0.00001)
    {
      continue;
    }
    auto f = 1.0 / a;
    auto s = ray.origin() - v0;
    auto u = f * glm::dot(s, h);
    if (u < 0.0 || u > 1.0)
    {
      continue;
    }
    auto q = glm::cross(s, e1);
    auto v = f * glm::dot(ray.direction(), q);
    if (v < 0.0 || u + v > 1.0)
    {
      continue;
    }
    auto t = f * glm::dot(e2, q);
    if (t > ray.t_min && t < closest_so_far)
    {
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

BBox Mesh::bbox() const
{
  BBox output_box;
  bool first_box = true;
  for (const auto &v : m_vertices)
  {
    if (first_box)
    {
      output_box = BBox(v, v);
      first_box = false;
    }
    else
    {
      output_box.expand(v);
    }
  }
  return output_box;
}

glm::vec3 Mesh::sample(const HitRecord &rec, Sampler *sampler) const
{
  auto index = static_cast<size_t>(sampler->get_1d() * (m_indices.size() / 3));
  const auto &v0 = m_vertices[m_indices[index]];
  const auto &v1 = m_vertices[m_indices[index + 1]];
  const auto &v2 = m_vertices[m_indices[index + 2]];
  auto u = sampler->get_1d();
  auto v = sampler->get_1d();
  if (u + v > 1)
  {
    u = 1 - u;
    v = 1 - v;
  }
  auto p = v0 + u * (v1 - v0) + v * (v2 - v0);
  return glm::normalize(p - rec.p);
}

float Mesh::pdf(const HitRecord &rec, const glm::vec3 &dir) const
{
  HitRecord test_rec;
  Ray test_ray(rec.p, dir);
  if (!hit(test_ray, test_rec))
  {
    return 0.0f;
  }
  auto distance_squared = test_rec.t * test_rec.t;
  auto cosine = glm::abs(glm::dot(dir, test_rec.normal));
  return distance_squared / (cosine * area);
}
