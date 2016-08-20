#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <psp2/gxm.h>
#include <psp2/gxm.h>

class Mesh
{
public:
  void				addIndex(uint16_t index);
  void				addVertex(const glm::vec3 & vertex);
  const				std::vector<uint16_t> & getIndices() const;
  const				std::vector<glm::vec3> & getVertices() const;
  bool				uploadToVram();
  const glm::vec3		*getVerticesPointer() const;
  const uint16_t		*getIndicesPointer() const;
  void				release();
  void				draw(SceGxmContext *context);
  
private:
  std::vector<uint16_t>		_indices;
  std::vector<glm::vec3>	_vertices;
  SceUID			_verticesUid;
  SceUID			_indicesUid;

  glm::vec3*			_pVertices = nullptr;
  uint16_t*			_pIndices = nullptr;
};
