#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "AssetsManager.hh"
#include <glm/gtc/type_ptr.hpp>

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

bool AssetsManager::addMesh(const std::string & path)
{
  return loadObj(path);
}

Mesh & AssetsManager::getMesh(const std::string & name)
{
  return _meshes[name];
}

bool AssetsManager::addMaterial(Material & material)
{
  _materials[material.name] = material;
  return true;
}
Material & AssetsManager::getMaterial(const std::string & name)
{
  return _materials[name];
}

bool AssetsManager::loadMaterial(const std::string & obPath, tinyobj::material_t & material)
{
  std::string path = obPath.substr(0, obPath.find_last_of("/")) + "/";

  debugNetPrintf(INFO, "loading material [%s]\n", material.name.c_str());

  Material mat;

  mat.name = material.name;
  mat.diffuse = glm::make_vec3(material.diffuse);
  mat.ambiant = glm::make_vec3(material.ambient);
  mat.specular = glm::make_vec3(material.specular);
  mat.map_diffuse = material.diffuse_texname;
  mat.shininess = material.shininess;

  _materials[material.name] = mat;
  // Check if we have to register texture

  debugNetPrintf(INFO, "loading texture:  [%s]\n", mat.map_diffuse.c_str());

  if (mat.map_diffuse != "" && _textures.find(mat.map_diffuse) == _textures.end())
  {
    Texture tex;
      debugNetPrintf(INFO, "loading texture [%s]\n", std::string(path + mat.map_diffuse).c_str());


    if (!tex.loadFromPng(path + mat.map_diffuse))
      return false;
    _textures[mat.map_diffuse] = tex;
  }

  return true;
}

bool AssetsManager::addShader(const std::string & name, Shader & shader)
{
  _shaders[name] = shader;
}

Shader & AssetsManager::getShader(const std::string & name)
{
  return _shaders[name];
}

bool AssetsManager::loadObj(const std::string & fileName)
{
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string err;
  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fileName.c_str());

  if (!ret)
  {
    debugNetPrintf(INFO, "Error loading : %s\n", fileName.c_str());
    return false;
  }


  for (size_t s = 0; s < shapes.size(); s++)
  {
      debugNetPrintf(INFO, "loading %s found : %s\n", fileName.c_str(), shapes[s].name.c_str());

    _meshes[shapes[s].name] = Mesh();

    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
    {
      int fv = shapes[s].mesh.num_face_vertices[f];

      for (size_t v = 0; v < fv; v++)
      {
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        float vx = attrib.vertices[3*idx.vertex_index+0];
        float vy = attrib.vertices[3*idx.vertex_index+1];
        float vz = attrib.vertices[3*idx.vertex_index+2];
        float nx = attrib.normals[3*idx.normal_index+0];
        float ny = attrib.normals[3*idx.normal_index+1];
        float nz = attrib.normals[3*idx.normal_index+2];
        float tx = attrib.texcoords[2*idx.texcoord_index+0];
        float ty = attrib.texcoords[2*idx.texcoord_index+1];

        _meshes[shapes[s].name].addVertex(glm::vec3(vx, vy, vz));
        _meshes[shapes[s].name].addNormal(glm::vec3(nx, ny, nz));
        _meshes[shapes[s].name].addTexCoord(glm::vec2(tx, 1 - ty)); // Inverse y texCoord
      }
      index_offset += fv;
    }
    _meshes[shapes[s].name].uploadToVram();
  }

  for (auto & mat : materials)
  {
    if (!loadMaterial(fileName, mat))
      return false;
    }
  return true;
}
