#include "hittable.h"

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
    double sqrt_discriminant = sqrt(discriminant);
    double root = (-b - sqrt_discriminant) / a;
    if (root < r.t_min || root > r.t_max)
    {
        root = (-b + sqrt_discriminant) / a;
        if (root < r.t_min || root > r.t_max)
        {
            return false;
        }
    }
    rec.t = root;
    rec.p = r.at(rec.t);
    rec.normal = (rec.p - center) / radius;
    return true;
}

bool Scene::hit(const Ray &r, HitRecord &rec) const
{
    HitRecord temp_rec;
    bool hit_anything = false;
    double closest_so_far = r.t_max;
    for (const auto &object : list)
    {
        if (object->hit(r, temp_rec))
        {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}

BBox Scene::bbox() const
{
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