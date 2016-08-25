#include <glm/gtc/matrix_inverse.hpp>
#include <debugnet.h>
#include "Ground.hh"

bool Ground::init(ObjLoader & loader, AssetsManager & manager, PhysicSystem & physicSystem)
{
  if (!loader.loadModel("app0:res/Mesh/Ground/ground.obj", _mesh, manager))
    return false;

  int numVertices = _mesh[0].getIndices().size();
  int numTriangles = numVertices / 3;

  debugNetPrintf(INFO, (char*)"num triangles : %d num vertices : %d\n", numTriangles, numVertices);
  
  int *indices = new int[_mesh[0].getIndices().size()];
  int i = 0;
  for (auto i : _mesh[0].getIndices())
    {
      indices[i] = (int)i;
      i++;
    }
  

  
  _indexVertexArrays = new btTriangleIndexVertexArray(numTriangles,
						      indices,
						      3*sizeof(int),
						      numVertices,
						      (btScalar*)&(_mesh[0].getVertices()[0].x),
						      3 * sizeof(float));


  
  
  bool useQuantizedAabbCompression = true;
  btCollisionShape* groundShape =
    new btBvhTriangleMeshShape(_indexVertexArrays,useQuantizedAabbCompression);
  
  
  btTransform tr;
  tr.setIdentity();
  tr.setOrigin(btVector3(0,0,0));  
  physicSystem.addCollisionShape(groundShape);



  physicSystem.localCreateRigidBody(0,tr,groundShape);
  
  
  return true;
}

void Ground::update(float elapsedTime, SceCtrlData & pad)
{
}

void Ground::draw(Shader & shader, Camera & camera, SceGxmContext *context, AssetsManager & manager)
{
  glm::mat4 m(1.0f);
  glm::mat4 mvp = camera.getProjectionMatrix() * camera.getViewMatrix() * m;
  glm::mat4 mv = camera.getViewMatrix() * m;  
  glm::mat3 n = glm::inverseTranspose(glm::mat3(mv));
  
  shader.bind(context);
  
  shader.setUniformMat4(VERTEX, "wvp", context, mvp);  
  shader.setUniformMat3(VERTEX, "normalMatrix", context, n);
  shader.setUniformVec3(FRAGMENT, "cameraPosition", context, camera.getPosition());
  shader.setUniformMat4(FRAGMENT, "model", context, m);

  for (auto & m : _mesh)
    {
      std::string diff = m.getMaterial().mapDiff;
      if (diff != "")
        shader.setUniformTexture(context, manager.getTexture(diff));
      m.draw(context);
    }
}

void Ground::release(PhysicSystem & system)
{
  for (auto & m : _mesh)
    m.release();
}
