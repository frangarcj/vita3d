#include <cstring>
#include <algorithm>
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

void Mesh::addNormal(const glm::vec3 &normal)
{
  _normals.push_back(normal);
}

void Mesh::addTexCoord(const glm::vec2 & texCoord)
{
  _texCoords.push_back(texCoord);
}

const std::vector<uint16_t> & Mesh::getIndices() const
{
  return _indices;
}

const std::vector<glm::vec3> & Mesh::getVertices() const
{
  return _vertices;
}

const std::vector<glm::vec3> & Mesh::getNormals() const
{
  return _normals;
}

const std::vector<glm::vec2> & Mesh::getTexCoords() const
{
  return _texCoords;
}

bool Mesh::uploadToVram()
{
  if (_indices.empty())
    {
      for (int i = 0; i < _vertices.size(); i++)
	{
	  _indices.push_back(i);
	}
    }
  
  debugNetPrintf(INFO, (char*)"Mesh::uploadToVram()\n");
  // Allocate vram
  _pVertices = static_cast<Vattrib*>(gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
						 _vertices.size() * sizeof(Vattrib),
						 4,
						 SCE_GXM_MEMORY_ATTRIB_READ,
						 &_verticesUid));

  debugNetPrintf(INFO, (char*)"Verices : %d TexCoords : %d\n",
		 _vertices.size(), _texCoords.size());
  
  _pIndices = static_cast<uint16_t*>(gpu_alloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
						_indices.size() * sizeof(uint16_t),
						2,
						SCE_GXM_MEMORY_ATTRIB_READ,
						&_indicesUid));

  debugNetPrintf(INFO, (char*)"Mesh::uploadToVram vertices allocated\n");
  
  // Copy vertices and indices to vram
  for (int i = 0; i < _vertices.size(); i++)
    {
      _pVertices[i] = Vattrib
	{
	  _vertices[i].x,
	  _vertices[i].y,
	  _vertices[i].z,
	  _normals[i].x,
	  _normals[i].y,
	  _normals[i].z,
	  _texCoords[i].x,
	  _texCoords[i].y
	};
    }
  
  memcpy(_pIndices, &_indices[0], _indices.size() * sizeof(uint16_t));  

  /*
  // Free memory for now
  _normals.clear();
  _vertices.clear();
  _texCoords.clear();
*/
  
  debugNetPrintf(INFO, (char*)"Mesh::uploadToVram vertices copied\n");
  
  return true;
}

const Vattrib		*Mesh::getVerticesPointer() const
{
  return _pVertices;
}

const uint16_t		*Mesh::getIndicesPointer() const
{
  return _pIndices;
}

void			Mesh::release()
{
  gpu_free(_verticesUid);
  gpu_free(_indicesUid);
}

void	Mesh::draw(SceGxmContext *context)
{
  sceGxmSetVertexStream(context, 0, _pVertices);
  sceGxmDraw(context,
	     SCE_GXM_PRIMITIVE_TRIANGLES,
	     SCE_GXM_INDEX_FORMAT_U16,
	     _pIndices,
	     _indices.size());
}
