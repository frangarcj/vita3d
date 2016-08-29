#include <glm/gtc/matrix_inverse.hpp>
#include <debugnet.h>
#include <string>
#include "Ground.hh"

bool Ground::init(AssetsManager & manager)
{
  Mesh & ground = manager.getMesh("ground");

  _collider.init(ground);

  btTransform tr;
  tr.setIdentity();
  tr.setOrigin(btVector3(0,0,0));

  //physicSystem.addCollisionShape(_collider.getCollisionShape());
  //physicSystem.localCreateRigidBody(0, tr, _collider.getCollisionShape());

  return true;
}

void Ground::update(float elapsed, SceCtrlData & pad)
{
}


void Ground::draw(Camera & camera, SceGxmContext *context, AssetsManager & manager)
{
  glm::mat4 m(1.0f);
  glm::mat4 mvp = camera.getProjectionMatrix() * camera.getViewMatrix() * m;
  glm::mat4 mv = camera.getViewMatrix() * m;
  glm::mat4 n = glm::inverseTranspose(m);

  Shader & shader = manager.getShader("light");
  Mesh & mesh = manager.getMesh("ground");
  Material & mat = manager.getMaterial("MAT_ground");

//  debugNetPrintf(INFO, "Material : %s\nTexture : %s\n", mat.name.c_str(), mat.map_diffuse.c_str());


  shader.bind(context);
  shader.setUniformMat4(VERTEX, "wvp", context, mvp);
  shader.setUniformMat4(VERTEX, "normalMatrix", context, n);
  shader.setUniformTexture(context, manager.getTexture(mat.map_diffuse));
  mesh.draw(context);
  
}

void Ground::release()
{
}

MeshCollider & Ground::getCollider()
{
  return _collider;
}
