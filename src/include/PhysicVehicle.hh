#pragma once

#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Vehicle/btRaycastVehicle.h>
#include "PhysicSystem.hh"

class PhysicVehicle
{
public:
  bool init(PhysicSystem & system);
  void update();
  void resetAcceleration();
  void accelerate();
  void turnRight();
  void turnLeft();
  void brake();

  btTransform getChassisTansform() const;
  btTransform getWheelTransform(int i) const;


private:
  int _rightIndex = 0;
  int _upIndex = 1;
  int _forwardIndex = 2;

  btVector3 _wheelDirectionCS0 = btVector3(0,-1,0);
  btVector3 _wheelAxleCS = btVector3(-1,0,0);

  const int _maxProxies = 32766;
  const int _maxOverlap = 65535;

  float	_gEngineForce = 0.f;
  float	_gBreakingForce = 0.f;

  float	_maxEngineForce = 1000.f;//this should be engine/velocity dependent
  float	_maxBreakingForce = 100.f;

  float	_gVehicleSteering = 0.f;
  float	_steeringIncrement = 0.04f;
  float	_steeringClamp = 0.3f;
  float	_wheelRadius = 0.5f;
  float	_wheelWidth = 0.4f;
  float	_wheelFriction = 1000;//BT_LARGE_FLOAT;
  float	_suspensionStiffness = 20.f;
  float	_suspensionDamping = 2.3f;
  float	_suspensionCompression = 4.4f;
  float	_rollInfluence = 0.1f;//1.0f;
  btScalar _suspensionRestLength = 0.6f;

  btRigidBody*					_carChassis;
  btRaycastVehicle::btVehicleTuning		_tuning;
  btVehicleRaycaster*				_vehicleRayCaster;
  btRaycastVehicle*				_vehicle;
  btCollisionShape*				_wheelShape;
};
