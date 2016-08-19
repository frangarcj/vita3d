#include <cstring>
#include <debugnet.h>
#include "Mesh.hh"
#include "Utils.hh"


void Mesh::addIndex(uint16_t index)
{
  _indices.push_back(index);
}

void Mesh::addVertex(const glm::vec3 & vertex)
{
  _vertices.push_back(vertex);
}

const std::vector<uint16_t> & Mesh::getIndices() const
{
  return _indices;
}

const std::vector<glm::vec3> & Mesh::getVertices() const
{
  return _vertices;
}

bool Mesh::uploadToVram()
{
  debugNetPrintf(INFO, (char*)"Mesh::uploadToVram()\n");
  // Allocate vram
  _pVertices = static_cast<glm::vec3*>(gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
						 _vertices.size() * sizeof(glm::vec3),
						 4,
						 SCE_GXM_MEMORY_ATTRIB_READ,
						 &_verticesUid));
  _pIndices = static_cast<uint16_t*>(gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
						_indices.size() * sizeof(uint16_t),
						2,
						SCE_GXM_MEMORY_ATTRIB_READ,
						&_indicesUid));

  debugNetPrintf(INFO, (char*)"Mesh::uploadToVram vertices allocated\n");
  
  // Copy vertices and indices to vram
  memcpy(_pVertices, &_vertices[0], _vertices.size() * sizeof(glm::vec3));
  memcpy(_pIndices, &_indices[0], _indices.size() * sizeof(uint16_t));  

  debugNetPrintf(INFO, (char*)"Mesh::uploadToVram vertices copied\n");
  
  return true;
}

const glm::vec3		*Mesh::getVerticesPointer() const
{
  return _pVertices;
}

const uint16_t		*Mesh::getIndicesPointer() const
{
  return _pIndices;
}
