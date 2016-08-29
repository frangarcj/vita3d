#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <psp2/gxm.h>
#include <string>


struct Vattrib
{
  float vx;
  float vy;
  float vz;
  float vnx;
  float vny;
  float vnz;
  float vtx;
  float vty;
};

class Mesh
{
public:
  void				addIndex(uint16_t index);
  void				addVertex(const glm::vec3 & vertex);
  void				addTexCoord(const glm::vec2 & texCoord);
  void				addNormal(const glm::vec3 & normal);
  const				std::vector<uint16_t> & getIndices() const;
  const				std::vector<glm::vec3> & getVertices() const;
  const				std::vector<glm::vec2> & getTexCoords() const;
  const				std::vector<glm::vec3> & getNormals() const;
  bool				uploadToVram();
  const Vattrib			*getVerticesPointer() const;
  const uint16_t		*getIndicesPointer() const;
  void				release();
  void				draw(SceGxmContext *context);
  
private:
  std::vector<uint16_t>		_indices;
  std::vector<glm::vec3>	_vertices;
  std::vector<glm::vec3>	_normals;
  std::vector<glm::vec2>	_texCoords;


  SceUID			_verticesUid;
  SceUID			_indicesUid;
  
  Vattrib*			_pVertices = nullptr;
  uint16_t*			_pIndices = nullptr;
};
