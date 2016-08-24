#pragma once

#include "Mesh.hh"
#include "Texture.hh"
#include <map>
#include <string>

class AssetsManager
{
public:
  bool	addTexture(const std::string & path);
  Texture & getTexture(const std::string & name);
  void	release();
  
private:
  std::map<std::string, Texture> _textures;
};
