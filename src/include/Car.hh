#pragma once

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_inverse.hpp>
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

class Car
{
public:
  bool init(ObjLoader & loader, AssetsManager & manager, PhysicSystem & system);
  void update(float elapsedTime, SceCtrlData & pad);
  void draw(Shader & shader, Camera & camera, SceGxmContext *context, AssetsManager & manager);
  void release(PhysicSystem & system);
  
private:
  glm::vec3 _position;
  glm::vec3 _rotation;
  std::vector<Mesh> _chassis;
  std::vector<Mesh> _wheel;

  btRigidBody*					_carChassis;

  btRaycastVehicle::btVehicleTuning		_tuning;
  btVehicleRaycaster*				_vehicleRayCaster;
  btRaycastVehicle*				_vehicle;
  btCollisionShape*				_wheelShape;
};
