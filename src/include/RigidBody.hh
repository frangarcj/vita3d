#pragma once

#include <btBulletDynamicsCommon.h>
#include "Mesh.hh"


class MeshCollider
{
public:
  void	init(const Mesh & mesh);
private:
  btTriangleIndexVertexArray	*_indexVertexArrays;
};
