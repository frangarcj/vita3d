#include "AssetsManager.hh"

bool AssetsManager::addTexture(const std::string & path)
{
  _textures[path] = Texture();
  return _textures[path].loadFromPng(path); 
}

Texture & AssetsManager::getTexture(const std::string & name)
{
  return _textures[name];
}

void	AssetsManager::release()
{
  for (auto & p : _textures)
    {
      p.second.release();
    }
}
