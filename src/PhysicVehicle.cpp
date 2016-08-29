#include "PhysicVehicle.hh"

#define CUBE_HALF_EXTENTS 1

bool PhysicVehicle::init(PhysicSystem & system)
{
  btCollisionShape* chassisShape = new btBoxShape(btVector3(1.f,0.5f, 2.f));
  btCompoundShape* compound = new btCompoundShape();

  system.addCollisionShape(chassisShape);
  system.addCollisionShape(compound);

  btTransform localTrans;
  localTrans.setIdentity();
  localTrans.setOrigin(btVector3(0,1,0));

  compound->addChildShape(localTrans,chassisShape);

  btTransform tr;
  tr.setIdentity();
  tr.setOrigin(btVector3(0,0.f,0));

  _carChassis = system.localCreateRigidBody(800,tr,compound); //chassisShape);
  _wheelShape = new btCylinderShapeX(btVector3(_wheelWidth, _wheelRadius, _wheelRadius));

   {
    _vehicleRayCaster = new btDefaultVehicleRaycaster(system.getWorld());
    _vehicle = new btRaycastVehicle(_tuning,_carChassis,_vehicleRayCaster);

    _carChassis->setActivationState(DISABLE_DEACTIVATION);
    system.getWorld()->addVehicle(_vehicle);

    float connectionHeight = 1.2f;
    bool isFrontWheel=true;

    _vehicle->setCoordinateSystem(_rightIndex, _upIndex, _forwardIndex);

    btVector3 connectionPointCS0(CUBE_HALF_EXTENTS -(0.3 * _wheelWidth), connectionHeight, 2 * CUBE_HALF_EXTENTS - _wheelRadius);
    _vehicle->addWheel(connectionPointCS0, _wheelDirectionCS0, _wheelAxleCS, _suspensionRestLength, _wheelRadius,_tuning,isFrontWheel);
    connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS + (0.3 * _wheelWidth), connectionHeight, 2 * CUBE_HALF_EXTENTS - _wheelRadius);
    _vehicle->addWheel(connectionPointCS0, _wheelDirectionCS0, _wheelAxleCS, _suspensionRestLength, _wheelRadius,_tuning,isFrontWheel);
    connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS + (0.3 * _wheelWidth), connectionHeight, -2 * CUBE_HALF_EXTENTS + _wheelRadius);
    isFrontWheel = false;
    _vehicle->addWheel(connectionPointCS0, _wheelDirectionCS0, _wheelAxleCS, _suspensionRestLength, _wheelRadius, _tuning, isFrontWheel);
    connectionPointCS0 = btVector3(CUBE_HALF_EXTENTS - (0.3 * _wheelWidth), connectionHeight, -2 * CUBE_HALF_EXTENTS + _wheelRadius);
    _vehicle->addWheel(connectionPointCS0, _wheelDirectionCS0, _wheelAxleCS, _suspensionRestLength, _wheelRadius,_tuning,isFrontWheel);

    for (int i=0; i <_vehicle->getNumWheels();i++)
      {
	btWheelInfo& wheel = _vehicle->getWheelInfo(i);
	wheel.m_suspensionStiffness = _suspensionStiffness;
	wheel.m_wheelsDampingRelaxation = _suspensionDamping;
	wheel.m_wheelsDampingCompression = _suspensionCompression;
	wheel.m_frictionSlip = _wheelFriction;
	wheel.m_rollInfluence = _rollInfluence;
      }
  }

  return true;
}

void PhysicVehicle::update()
{
  int wheelIndex = 2;
  _vehicle->applyEngineForce(_gEngineForce,wheelIndex);
  _vehicle->setBrake(_gBreakingForce,wheelIndex);
  wheelIndex = 3;
  _vehicle->applyEngineForce(_gEngineForce,wheelIndex);
  _vehicle->setBrake(_gBreakingForce,wheelIndex);


  wheelIndex = 0;
  _vehicle->setSteeringValue(_gVehicleSteering,wheelIndex);
  wheelIndex = 1;
  _vehicle->setSteeringValue(_gVehicleSteering,wheelIndex);
}

void PhysicVehicle::resetAcceleration()
{
  _gEngineForce = 0.0f;
  _gBreakingForce = 0.0f;
}

void PhysicVehicle::brake()
{
  _gBreakingForce = _maxBreakingForce;
  _gEngineForce = 0.f;
}

void PhysicVehicle::accelerate()
{
  _gEngineForce = _maxEngineForce;
  _gBreakingForce = 0.f;

}

void PhysicVehicle::turnRight()
{
  _gVehicleSteering -= _steeringIncrement;
  if (_gVehicleSteering < -_steeringClamp)
    _gVehicleSteering = -_steeringClamp;
}

void PhysicVehicle::turnLeft()
{
  _gVehicleSteering += _steeringIncrement;
  if (_gVehicleSteering > _steeringClamp)
    _gVehicleSteering = _steeringClamp;
}

btTransform PhysicVehicle::getChassisTansform() const
{
  btTransform transform;
  _carChassis->getMotionState()->getWorldTransform(transform);

  return transform;
}

btTransform PhysicVehicle::getWheelTransform(int i) const
{
  return _vehicle->getWheelTransformWS(i);
}
