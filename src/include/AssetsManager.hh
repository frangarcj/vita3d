#pragma once

#include "Mesh.hh"
#include "Texture.hh"
#include <map>
#include <string>
#include "Material.hh"
#include "Shader.hh"
#include "tiny_obj_loader.h"


using namespace tinyobj;;

class AssetsManager
{
public:
  bool	addTexture(const std::string & path);
  Texture & getTexture(const std::string & name);

  bool addMesh(const std::string & path);
  Mesh & getMesh(const std::string & name);

  bool addMaterial(Material & material);
  Material & getMaterial(const std::string & name);

  bool addShader(const std::string & name, Shader & shader);
  Shader & getShader(const std::string & name);
  
  void	release();

private:
  bool loadObj(const std::string & fileName);
  bool loadMaterial(const std::string & obPath, tinyobj::material_t & material);

private:
  std::map<std::string, Texture> _textures;
  std::map<std::string, Mesh> _meshes;
  std::map<std::string, Material> _materials;
  std::map<std::string, Shader> _shaders;
};
