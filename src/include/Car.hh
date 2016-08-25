#pragma once

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <btBulletDynamicsCommon.h>
#include <psp2/types.h>
#include <psp2/ctrl.h>
#include "Mesh.hh"
#include "Shader.hh"
#include "Camera.hh"
#include "AssetsManager.hh"
#include "ObjLoader.hh"

class Car
{
public:
  bool init(ObjLoader & loader, AssetsManager & manager, btDiscreteDynamicsWorld *world);
  void update(float elapsedTime, SceCtrlData & pad);
  void draw(Shader & shader, Camera & camera, SceGxmContext *context, AssetsManager & manager);
  void release(btDiscreteDynamicsWorld *world);
  
private:
  glm::vec3 _position;
  glm::vec3 _rotation;
  std::vector<Mesh> _meshes;
  btCollisionShape* _shape;
  btRigidBody*	_rigidBody;
};
