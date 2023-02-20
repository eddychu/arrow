#pragma once
#include "ray.h"
#include "bbox.h"
#include <vector>
#include <memory>
struct HitRecord
{
    float t;
    glm::vec3 p;
    glm::vec3 normal;
};

class Hittable
{
public:
    virtual ~Hittable()
    {
    }
    virtual bool hit(const Ray &r, HitRecord &rec) const = 0;
    virtual BBox bbox() const = 0;
};

class Sphere : public Hittable
{
public:
    Sphere() {}
    Sphere(glm::vec3 cen, float r) : center(cen), radius(r){};
    virtual bool hit(const Ray &r, HitRecord &rec) const;
    virtual BBox bbox() const override
    {
        return BBox(center - glm::vec3(radius), center + glm::vec3(radius));
    }
    glm::vec3 center;
    float radius;
};

class Scene : public Hittable
{
public:
    Scene() {}
    void add(std::unique_ptr<Hittable> h)
    {
        list.push_back(std::move(h));
    }
    virtual bool hit(const Ray &r, HitRecord &rec) const;
    virtual BBox bbox() const override;
    std::vector<std::unique_ptr<Hittable>> list;
};