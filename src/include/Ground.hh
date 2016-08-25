#pragma once

#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Vehicle/btRaycastVehicle.h>
#include <psp2/types.h>
#include <psp2/ctrl.h>
#include "Mesh.hh"
#include "Shader.hh"
#include "Camera.hh"
#include "AssetsManager.hh"
#include "ObjLoader.hh"
#include "PhysicSystem.hh"
#include <stdlib.h>

class Ground
{
public:
  bool init(ObjLoader & loader, AssetsManager & manager, PhysicSystem & physicSystem);
  void update(float elapsedTime, SceCtrlData & pad);
  void draw(Shader & shader, Camera & camera, SceGxmContext *context, AssetsManager & manager);
  void release(PhysicSystem & system);
  
private:
  std::vector<Mesh>				_mesh;
  btVector3*					_vertices;
  btTriangleIndexVertexArray*			_indexVertexArrays;
};
