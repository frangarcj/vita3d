#include <debugnet.h>
#include "MeshCollider.hh"

void MeshCollider::init(const Mesh & mesh)
{
  int numVertices = mesh.getIndices().size();
  int numTriangles = numVertices / 3;
  
  int *indices = new int[mesh.getIndices().size()];
  int i = 0;
  for (auto i : mesh.getIndices())
    {
      indices[i] = (int)i;
      i++;
    }

  _indexVertexArrays = new btTriangleIndexVertexArray(numTriangles,
						      indices,
						      3 * sizeof(int),
						      numVertices,
						      (btScalar*)&(mesh.getVertices()[0].x),
						      3 * sizeof(float));

  _shape =
    new btBvhTriangleMeshShape(_indexVertexArrays, true);
}

btCollisionShape* MeshCollider::getCollisionShape()
{
  return _shape;
}
