#include "Car.hh"
#include <debugnet.h>

int rightIndex = 0;
int upIndex = 1;
int forwardIndex = 2;

btVector3 wheelDirectionCS0(0,-1,0);
btVector3 wheelAxleCS(-1,0,0);

const int maxProxies = 32766;
const int maxOverlap = 65535;

float	gEngineForce = 0.f;
float	gBreakingForce = 0.f;

float	maxEngineForce = 1000.f;//this should be engine/velocity dependent
float	maxBreakingForce = 100.f;

float	gVehicleSteering = 0.f;
float	steeringIncrement = 0.04f;
float	steeringClamp = 0.3f;
float	wheelRadius = 0.5f;
float	wheelWidth = 0.4f;
float	wheelFriction = 1000;//BT_LARGE_FLOAT;
float	suspensionStiffness = 20.f;
float	suspensionDamping = 2.3f;
float	suspensionCompression = 4.4f;
float	rollInfluence = 0.1f;//1.0f;


btScalar suspensionRestLength(0.6);

#define CUBE_HALF_EXTENTS 1

glm::mat4 m(1.0f);


bool Car::init(ObjLoader & loader, AssetsManager & manager, PhysicSystem & system)
{
  _position = glm::vec3(0.f, 0.f, 0.f);
  _rotation = glm::vec3(0.f);

  if (!loader.loadModel("app0:res/Mesh/car/chassis.obj", _chassis, manager))
    return false;
  

  btCollisionShape* chassisShape = new btBoxShape(btVector3(1.f,0.5f,2.f));
  system.addCollisionShape(chassisShape); 
  btCompoundShape* compound = new btCompoundShape();

  system.addCollisionShape(compound); 

  btTransform localTrans;
  localTrans.setIdentity();
  localTrans.setOrigin(btVector3(0,1,0));

  compound->addChildShape(localTrans,chassisShape);

  btTransform tr;
  tr.setIdentity();
  tr.setOrigin(btVector3(0,0.f,0));

  _carChassis = system.localCreateRigidBody(800,tr,compound); //chassisShape);
  //m_carChassis->setDamping(0.2,0.2);

  _wheelShape = new btCylinderShapeX(btVector3(wheelWidth,wheelRadius,wheelRadius));

  
  // Vehicle creation
  {
    _vehicleRayCaster = new btDefaultVehicleRaycaster(system.getWorld());
    _vehicle = new btRaycastVehicle(_tuning,_carChassis,_vehicleRayCaster);

    ///never deactivate the vehicle
    _carChassis->setActivationState(DISABLE_DEACTIVATION);

    system.getWorld()->addVehicle(_vehicle);
    

    float connectionHeight = 1.2f;


    bool isFrontWheel=true;

    //choose coordinate system
    _vehicle->setCoordinateSystem(rightIndex,upIndex,forwardIndex);

    btVector3 connectionPointCS0(CUBE_HALF_EXTENTS-(0.3*wheelWidth),connectionHeight,2*CUBE_HALF_EXTENTS-wheelRadius);
    _vehicle->addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,suspensionRestLength,wheelRadius,_tuning,isFrontWheel);
    connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS+(0.3*wheelWidth),connectionHeight,2*CUBE_HALF_EXTENTS-wheelRadius);   
    _vehicle->addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,suspensionRestLength,wheelRadius,_tuning,isFrontWheel);
    connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS+(0.3*wheelWidth),connectionHeight,-2*CUBE_HALF_EXTENTS+wheelRadius);
    isFrontWheel = false;
    _vehicle->addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,suspensionRestLength,wheelRadius,_tuning,isFrontWheel);
    connectionPointCS0 = btVector3(CUBE_HALF_EXTENTS-(0.3*wheelWidth),connectionHeight,-2*CUBE_HALF_EXTENTS+wheelRadius);
    _vehicle->addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,suspensionRestLength,wheelRadius,_tuning,isFrontWheel);
    

    for (int i=0; i <_vehicle->getNumWheels();i++)
      {
	btWheelInfo& wheel = _vehicle->getWheelInfo(i);
	wheel.m_suspensionStiffness = suspensionStiffness;
	wheel.m_wheelsDampingRelaxation = suspensionDamping;
	wheel.m_wheelsDampingCompression = suspensionCompression;
	wheel.m_frictionSlip = wheelFriction;
	wheel.m_rollInfluence = rollInfluence;
      }
  }
  
  return true;
}

void Car::update(float elapsedTime, SceCtrlData & pad)
{
  gEngineForce = 0;
  if (pad.buttons & SCE_CTRL_CROSS)
    {
      gEngineForce = maxEngineForce;
      gBreakingForce = 0.f;
    }
  if (pad.buttons & SCE_CTRL_SQUARE)
    {
      gBreakingForce = maxBreakingForce;
      gEngineForce = 0.f;
    }
  if (pad.buttons & SCE_CTRL_LEFT)
    {
      gVehicleSteering += steeringIncrement;
      if (gVehicleSteering > steeringClamp)
	gVehicleSteering = steeringClamp;
    }
  if (pad.buttons & SCE_CTRL_RIGHT)
    {
      gVehicleSteering -= steeringIncrement;
      if (gVehicleSteering < -steeringClamp)
	gVehicleSteering = -steeringClamp;
    }
  
  {
    int wheelIndex = 2;
    _vehicle->applyEngineForce(gEngineForce,wheelIndex);
    _vehicle->setBrake(gBreakingForce,wheelIndex);
    wheelIndex = 3;
    _vehicle->applyEngineForce(gEngineForce,wheelIndex);
    _vehicle->setBrake(gBreakingForce,wheelIndex);


    wheelIndex = 0;
    _vehicle->setSteeringValue(gVehicleSteering,wheelIndex);
    wheelIndex = 1;
    _vehicle->setSteeringValue(gVehicleSteering,wheelIndex);

  }

  btTransform chassisTrans;
  _carChassis->getMotionState()->getWorldTransform(chassisTrans);

  _position.x = chassisTrans.getOrigin().getX();
  _position.y = chassisTrans.getOrigin().getY();
  _position.z = chassisTrans.getOrigin().getZ();

  _rotation.x =  chassisTrans.getRotation().getX();
  _rotation.y =  chassisTrans.getRotation().getY();
  _rotation.z =  chassisTrans.getRotation().getZ();

  chassisTrans.getOpenGLMatrix(&m[0][0]);
  //  _position = chassisTrans.getOrigin()
  
}

void Car::draw(Shader & shader, Camera & camera, SceGxmContext *context, AssetsManager & manager)
{
  


  //  m = glm::yawPitchRoll(_rotation.x, _rotation.y, _rotation.z);
  //m = glm::translate(m, _position);

  
  glm::mat4 mvp = camera.getProjectionMatrix() * camera.getViewMatrix() * m;
  glm::mat4 mv = camera.getViewMatrix() * m;  
  glm::mat3 n = glm::inverseTranspose(glm::mat3(mv));

  shader.bind(context);
  
  shader.setUniformMat4(VERTEX, "wvp", context, mvp);  
  shader.setUniformMat3(VERTEX, "normalMatrix", context, n);
  shader.setUniformVec3(FRAGMENT, "cameraPosition", context, camera.getPosition());
  shader.setUniformMat4(FRAGMENT, "model", context, m);

  for (auto & m : _chassis)
    {
      std::string diff = m.getMaterial().mapDiff;
      if (diff != "")
        shader.setUniformTexture(context, manager.getTexture(diff));
      m.draw(context);
    }
}

void Car::release(PhysicSystem & system)
{ 
  for (auto & m : _chassis)
    m.release();
}
