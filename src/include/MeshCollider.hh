#pragma once

#include <btBulletDynamicsCommon.h>
#include "Mesh.hh"

class MeshCollider
{
public:
  void	init(const Mesh & mesh);
  btCollisionShape *getCollisionShape();
  
private:
  btTriangleIndexVertexArray	*_indexVertexArrays = nullptr;
  btCollisionShape*		_shape = nullptr;
};
