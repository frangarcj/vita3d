#pragma once

#include <string>
#include <glm/glm.hpp>

struct Material
{
  std::string	name;  
  glm::vec3	ambiant;
  glm::vec3	diffuse;
  glm::vec3	specular;
  float		shininess;
  std::string	map_diffuse;
  float		specularComponent;
};
