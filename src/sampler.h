#pragma once
#include <glm/glm.hpp>
#include <random>
class Sampler {
public:
  virtual ~Sampler() = default;
  virtual float get_1d() = 0;
  virtual glm::vec2 get_2d() = 0;
};

class RandomSampler : public Sampler {
public:
  RandomSampler() : m_gen(m_rd()), m_dis(0.0f, 1.0f) {}
  virtual float get_1d() override { return m_dis(m_gen); }
  virtual glm::vec2 get_2d() override {
    return glm::vec2(m_dis(m_gen), m_dis(m_gen));
  }

private:
  std::random_device m_rd;
  std::mt19937 m_gen;
  std::uniform_real_distribution<float> m_dis;
};