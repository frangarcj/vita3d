#include <sstream>
#include <iterator>
#include <psp2/io/fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <debugnet.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "ObjLoader.hh"

bool	ObjLoader::loadModel(const std::string & fileName, std::vector<Mesh> & model)
{
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string err;
  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fileName.c_str());

  if (!ret)
    return false;

  for (size_t s = 0; s < shapes.size(); s++) {
    model.push_back(Mesh());

    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      int fv = shapes[s].mesh.num_face_vertices[f];

      // Loop over vertices in the face.
      for (size_t v = 0; v < fv; v++) {
	// access to vertex
	tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
	float vx = attrib.vertices[3*idx.vertex_index+0];
	float vy = attrib.vertices[3*idx.vertex_index+1];
	float vz = attrib.vertices[3*idx.vertex_index+2];
	float nx = attrib.normals[3*idx.normal_index+0];
	float ny = attrib.normals[3*idx.normal_index+1];
	float nz = attrib.normals[3*idx.normal_index+2];
	float tx = attrib.texcoords[2*idx.texcoord_index+0];
	float ty = attrib.texcoords[2*idx.texcoord_index+1];

	model.at(s).addVertex(glm::vec3(vx, vy, vz));
	model.at(s).addTexCoord(glm::vec2(tx, ty));
      }
      index_offset += fv;

      // per-face material
      //      shapes[s].mesh.material_ids[f];
    }
  }

  for (auto & m : model)
    {
      m.uploadToVram();
    }
  
  return true;
}
