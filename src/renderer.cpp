#include "renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

void Renderer::save(const std::string &filename) const {
  std::vector<unsigned char> data(m_width * m_height * 3);
  for (int i = 0; i < m_width * m_height; i++) {
    data[i * 3 + 0] = glm::clamp(m_buffer[i].x * 255.0f, 0.0f, 255.0f);
    data[i * 3 + 1] = glm::clamp(m_buffer[i].y * 255.0f, 0.0f, 255.0f);
    data[i * 3 + 2] = glm::clamp(m_buffer[i].z * 255.0f, 0.0f, 255.0f);
  }
  stbi_write_png(filename.c_str(), m_width, m_height, 3, data.data(), 0);
}