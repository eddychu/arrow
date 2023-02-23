

#include "pdf.h"
#include "hittable.h"
glm::vec3 HitablePDF::sample(const HitRecord& rec, Sampler* sampler) const  {
        return hitable->sample(rec, sampler);
    }
float HitablePDF::value(const HitRecord& rec, const glm::vec3& dir) const  {
    return hitable->pdf(rec, dir);
}

glm::vec3 MixPDF::sample(const HitRecord& rec, Sampler* sampler) const {
        if(sampler->get_1d() < 0.5f) {
            return p0->sample(rec, sampler);
        } else {
            return p1->sample(rec, sampler);
        }
    }

float MixPDF::value(const HitRecord& rec, const glm::vec3& dir) const  {
        return 0.5f * p0->value(rec, dir) + 0.5f * p1->value(rec, dir);
    }