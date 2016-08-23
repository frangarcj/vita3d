#pragma once

#include <string>
#include <glm/glm.hpp>

struct Material
{
  glm::vec4	ambiant;
  glm::vec4	diffuse;
  glm::vec4	specular;
  float		specularComponent;
  std::string	map_opacity;
  std::string	map_diffuse;
};
