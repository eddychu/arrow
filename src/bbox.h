#pragma once

#include "ray.h"
#include <limits>
class BBox
{
public:
    BBox()
    {
        max_ = glm::vec3(std::numeric_limits<double>::min(), std::numeric_limits<double>::min(), std::numeric_limits<double>::min());
        min_ = glm::vec3(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
    }
    BBox(const glm::vec3 &a, const glm::vec3 &b)
    {
        min_ = a;
        max_ = b;
    }


    glm::vec3 min() const { return min_; }
    glm::vec3 max() const { return max_; }

    glm::vec3 center() const { return (min_ + max_) / 2.0f; }

    void expand(const glm::vec3 &p)
    {
        min_.x = glm::min(min_.x, p.x);
        min_.y = glm::min(min_.y, p.y);
        min_.z = glm::min(min_.z, p.z);
        max_.x = glm::max(max_.x, p.x);
        max_.y = glm::max(max_.y, p.y);
        max_.z = glm::max(max_.z, p.z);
    }

    void expand(const BBox &b)
    {
        expand(b.min());
        expand(b.max());
    }

    float surface_area() const
    {
        glm::vec3 d = max_ - min_;
        return 2.0f * (d.x * d.y + d.x * d.z + d.y * d.z);
    }

    int max_extent() const
    {
        glm::vec3 d = max_ - min_;
        if (d.x > d.y && d.x > d.z)
            return 0;
        else if (d.y > d.z)
            return 1;
        else
            return 2;
    }

    bool overlap(BBox &b) const
    {
        if (max_.x < b.min_.x || min_.x > b.max_.x)
            return false;
        if (max_.y < b.min_.y || min_.y > b.max_.y)
            return false;
        if (max_.z < b.min_.z || min_.z > b.max_.z)
            return false;
        return true;
    }

    bool inside(const glm::vec3 &p) const
    {
        return (p.x >= min_.x && p.x <= max_.x &&
                p.y >= min_.y && p.y <= max_.y &&
                p.z >= min_.z && p.z <= max_.z);
    }

    bool hit(const Ray &ray) const
    {
        auto t0 = ray.t_min;
        auto t1 = ray.t_max;

        for (int i = 0; i < 3; i++)
        {
            double inv_d = 1.0 / ray.direction()[i];
            double t_near = (min_[i] - ray.origin()[i]) * inv_d;
            double t_far = (max_[i] - ray.origin()[i]) * inv_d;
            if (inv_d < 0.0f)
                std::swap(t_near, t_far);
            if (t0 > t_far || t_near > t1)
                return false;
            t0 = glm::max(t0, t_near);
            t1 = glm::min(t1, t_far);
            if (t0 > t1)
                return false;
        }
        return true;
    }

private:
    glm::vec3 min_;
    glm::vec3 max_;
};
