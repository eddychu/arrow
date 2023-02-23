#pragma once
#include "record.h"
#include "accel.h"
#include "sampler.h"
#include "sampling.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <memory>
class Hittable;
class PDF {
    public:
    virtual glm::vec3 sample(const HitRecord& rec, Sampler* sampler) const = 0;
    virtual float value(const HitRecord& rec, const glm::vec3& dir) const = 0;
};


class CosinePDF : public PDF {
    public:
    virtual glm::vec3 sample(const HitRecord& rec, Sampler* sampler) const override {
        auto r1 = sampler->get_1d();
        auto r2 = sampler->get_1d();
        auto sample_dir = sample_cosine_weighted_hemisphere(r1, r2);
        // create a new coordinate system based on the normal
        auto w = rec.normal;
        auto a =
            w.x > 0.9f ? glm::vec3(0.0, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
        auto v = glm::normalize(glm::cross(w, a));
        auto u = glm::cross(w, v);
        auto dir = u * sample_dir.x + v * sample_dir.y + w * sample_dir.z;
        dir = glm::normalize(dir);
        return dir;
    }
    virtual float value(const HitRecord& rec, const glm::vec3& dir) const override {
        float cosine = glm::dot(glm::normalize(dir), rec.normal);
        if(cosine > 0) {
            return cosine / glm::pi<float>();
        } else {
            return 0;
        }
    }
};

class HitablePDF : public PDF {
    public:
    HitablePDF(const std::unique_ptr<Hittable>& hitable) : hitable(hitable) {}
    virtual glm::vec3 sample(const HitRecord& rec, Sampler* sampler) const override;
    virtual float value(const HitRecord& rec, const glm::vec3& dir) const override;
    const std::unique_ptr<Hittable>& hitable;
};

class MixPDF: public PDF {
    public:
    MixPDF(PDF* p0_, PDF* p1_) : p0(p0_), p1(p1_) {
    }
    virtual glm::vec3 sample(const HitRecord& rec, Sampler* sampler) const override;
    virtual float value(const HitRecord& rec, const glm::vec3& dir) const override;
    PDF* p0;
    PDF* p1;
};